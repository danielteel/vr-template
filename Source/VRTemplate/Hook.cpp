#include "Hook.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GrabberComponent.h"

UHook::UHook() {

}


void UHook::BeginDestroy() {
	Super::BeginDestroy();
	for (const TPair<UPrimitiveComponent*, UPhysicsConstraintComponent*>& pair : ConstraintsComponents) {
		if (pair.Value) {
			pair.Value->BreakConstraint();
			pair.Value->DestroyComponent();
		}
	}
	ConstraintsComponents.Reset();
}

void UHook::GrabStart_Implementation(UGrabberComponent* hand) {

}
void UHook::GrabEnd_Implementation(UGrabberComponent* hand) {

}
void UHook::InputButton1_Implementation(UGrabberComponent* hand) {
	UGrabberComponent* otherGrabber = hand->GetOtherGrabber();
	if (!otherGrabber) return;//Somehow cant find the other hand

	UPrimitiveComponent* otherGrabbedComponent = otherGrabber->GetGrabbedPrimitiveComponent();
	if (!otherGrabbedComponent) return;//Not a "physical" component

	if (ConstraintsComponents.Contains(otherGrabbedComponent)) return;//Already attached

	if (!otherGrabbedComponent->DoesSocketExist(FName("Hook"))) return;


	FVector otherGrabbedSocketLocation = otherGrabbedComponent->GetSocketLocation(FName("Hook"));
	FVector hookSocketLocation = GetSocketLocation(FName("Hook"));

	otherGrabbedComponent->AddWorldOffset(hookSocketLocation - otherGrabbedSocketLocation, false, nullptr, ETeleportType::TeleportPhysics);

	UPhysicsConstraintComponent* physicsConstraint = NewObject<UPhysicsConstraintComponent>(this);
	physicsConstraint->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	physicsConstraint->SetWorldLocation(hookSocketLocation);
	FConstraintInstance ConstraintInstance;
	ConstraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Locked;
	ConstraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Locked;
	ConstraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Locked;
	ConstraintInstance.ProfileInstance.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Free;
	ConstraintInstance.ProfileInstance.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Free;
	ConstraintInstance.ProfileInstance.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Free;
	ConstraintInstance.ProfileInstance.bDisableCollision = true;
	ConstraintInstance.ProfileInstance.bEnableProjection = true;
	physicsConstraint->ConstraintInstance = ConstraintInstance;
	physicsConstraint->InitComponentConstraint();


	physicsConstraint->SetConstrainedComponents(this, NAME_None, otherGrabbedComponent, NAME_None);

	ConstraintsComponents.Add(otherGrabbedComponent, physicsConstraint);

}
void UHook::InputButton2_Implementation(UGrabberComponent* hand) {
	UGrabberComponent* otherGrabber = hand->GetOtherGrabber();
	if (!otherGrabber) return;//Somehow cant find the other hand

	UPrimitiveComponent* otherGrabbedComponent = otherGrabber->GetGrabbedPrimitiveComponent();
	if (!otherGrabbedComponent) return;//Not a "physical" component

	if (!ConstraintsComponents.Contains(otherGrabbedComponent)) return;//Not attached

	UPhysicsConstraintComponent** constraintPtr = ConstraintsComponents.Find(otherGrabbedComponent);
	if (constraintPtr) {
		UPhysicsConstraintComponent* constraint = *constraintPtr;
		constraint->BreakConstraint();
		constraint->DestroyComponent();
	}

	ConstraintsComponents.Remove(otherGrabbedComponent);
}