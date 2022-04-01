#include "Hook.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GrabberComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


void UHook::OnDestroyPhysicsState() {
	for (const TPair<UPrimitiveComponent*, UPhysicsConstraintComponent*>& pair : ConstraintsComponents) {
		if (pair.Value) {
			pair.Value->BreakConstraint();
			pair.Value->DestroyComponent();
		}
	}
	ConstraintsComponents.Reset();
	Super::OnDestroyPhysicsState();
}

void UHook::GrabTick_Implementation(UGrabberComponent* hand, float deltaTime) {
	UGrabberComponent* otherGrabber = hand->GetOtherGrabber();
	if (!otherGrabber) {
		LastSeenOtherGrabbed = nullptr;
		WasJustBroken = false;
		return;//Somehow cant find the other hand
	}

	UPrimitiveComponent* otherGrabbedComponent = otherGrabber->GetGrabbedPrimitiveComponent();
	if (!otherGrabbedComponent) {
		LastSeenOtherGrabbed = nullptr;
		WasJustBroken = false;
		return;//Not a "physical" component
	}

	if (LastSeenOtherGrabbed == nullptr || LastSeenOtherGrabbed != otherGrabbedComponent) {
		WasJustBroken = false;
		LastSeenOtherGrabbed = otherGrabbedComponent;
		GrabbedDifference = hand->GetComponentLocation() - otherGrabber->GetComponentLocation();
		InitialGrabberDistance = FVector::Dist(hand->GetComponentLocation(), otherGrabber->GetComponentLocation());
	}

	if (WasJustBroken && LastSeenOtherGrabbed) {
		float dist = FVector::Dist(hand->GetComponentLocation() - GrabbedDifference, otherGrabber->GetComponentLocation());
		if (dist >= HookBreakResetDistance) {
			WasJustBroken = false;
		}
	} else {
		if (ConstraintsComponents.Contains(otherGrabbedComponent)) {
			//Component is attached, check to see if we need to detach it
			float dist = FVector::Dist(hand->GetComponentLocation() - GrabbedDifference, otherGrabber->GetComponentLocation());
			float grabbersDistance = FVector::Dist(hand->GetComponentLocation(), otherGrabber->GetComponentLocation());

			if (dist >= HookBreakDistance && grabbersDistance >=InitialGrabberDistance) {
				//Far enough apart, disconnect
				UPhysicsConstraintComponent** constraintPtr = ConstraintsComponents.Find(otherGrabbedComponent);
				if (constraintPtr) {
					UPhysicsConstraintComponent* constraint = *constraintPtr;
					if (constraint) {
						constraint->BreakConstraint();
						constraint->DestroyComponent();
					}
				}
				WasJustBroken = true;
				ConstraintsComponents.Remove(otherGrabbedComponent);

				if (DetachNoise) UGameplayStatics::PlaySound2D(GetWorld(), DetachNoise);
			}

		} else {
			//Component is not attached, check to see if we need to attach it
			if (!otherGrabbedComponent->DoesSocketExist(FName("Hookable"))) return;//Component doesnt have a Hookable socket

			FVector hookableSocketLocation = otherGrabbedComponent->GetSocketLocation(FName("Hookable"));
			FVector hookSocketLocation = GetSocketLocation(FName("Hook"));

			float distBetweenSockets = FVector::Dist(hookSocketLocation, hookableSocketLocation);
			if (distBetweenSockets <= HookUpDistance) {
				//Close enough, connect them
				LastSeenOtherGrabbed = nullptr;
				otherGrabbedComponent->AddWorldOffset(hookSocketLocation - hookableSocketLocation, false, nullptr, ETeleportType::ResetPhysics);

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

				if (AttachNoise) UGameplayStatics::PlaySound2D(GetWorld(), AttachNoise);
			}
		}
	}
}