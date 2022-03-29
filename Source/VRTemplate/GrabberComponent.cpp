// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabberComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "DrawDebugHelpers.h"

UGrabberComponent::UGrabberComponent() {
	PrimaryComponentTick.bCanEverTick = true;


}

void UGrabberComponent::BeginPlay() {
	Super::BeginPlay();

	PhysicsConstraint = NewObject<UPhysicsConstraintComponent>(this, FName("PhysicsHandle"));
	PhysicsConstraint->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PhysicsConstraint->SetWorldLocation(GetComponentLocation());


	FConstraintInstance ConstraintInstance;

	ConstraintInstance.ProfileInstance.LinearLimit.Limit = 0.1f;
	ConstraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Limited;
	ConstraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Limited;
	ConstraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Limited;
	ConstraintInstance.ProfileInstance.LinearLimit.Stiffness = 5000.0f;
	ConstraintInstance.ProfileInstance.LinearLimit.ContactDistance = 1.0f;
	ConstraintInstance.ProfileInstance.LinearLimit.bSoftConstraint = true;

	ConstraintInstance.ProfileInstance.ConeLimit.Swing1LimitDegrees = 0.1f;
	ConstraintInstance.ProfileInstance.ConeLimit.Swing2LimitDegrees = 0.1f;
	ConstraintInstance.ProfileInstance.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Limited;
	ConstraintInstance.ProfileInstance.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Limited;
	ConstraintInstance.ProfileInstance.ConeLimit.Stiffness = 150.0f;
	ConstraintInstance.ProfileInstance.ConeLimit.Damping = 25.0f;

	ConstraintInstance.ProfileInstance.TwistLimit.TwistLimitDegrees = 0.1f;
	ConstraintInstance.ProfileInstance.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Limited;
	ConstraintInstance.ProfileInstance.TwistLimit.Stiffness = 150.f;
	ConstraintInstance.ProfileInstance.TwistLimit.Damping = 25.0f;

	ConstraintInstance.ProfileInstance.bDisableCollision=true;
	ConstraintInstance.ProfileInstance.bEnableProjection = false;

	PhysicsConstraint->ConstraintInstance = ConstraintInstance;
	PhysicsConstraint->InitComponentConstraint();



}

void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawDebugLine(GetWorld(), GetComponentLocation(), PhysicsConstraint->GetComponentLocation(), FColor::Red, false, 0, 0, 1.0f);
	if (GrabbedComponent){
		if (ActorProxy) {
			IGrabbable::Execute_GrabTick(ActorProxy, GetHandController());
		} else {
			IGrabbable::Execute_GrabTick(GrabbedComponent, GetHandController());
		}
	}
}

UPrimitiveComponent* UGrabberComponent::GetHandController() {
	return Cast<UPrimitiveComponent>(this->GetAttachParent());
}

UPrimitiveComponent* UGrabberComponent::GetComponentToGrab(AActor*& actorProxy) {
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	TArray<AActor*> actorsToIgnore;
	TArray<UPrimitiveComponent*> outComponents;

	UPrimitiveComponent* componentToGrab = nullptr;
	FVector ourLocation = GetComponentLocation();

	actorProxy = nullptr;

	bool result = UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), ourLocation, GrabDistance, objectTypes, nullptr, actorsToIgnore, outComponents);
	if (result) {
		float distanceToGrab = 0.0f;
		for (int i = 0; i < outComponents.Num(); i++) {

			bool isGrabbableObject = outComponents[i]->Implements<UGrabbable>();

			if (isGrabbableObject) {
				EGrabType grabType = IGrabbable::Execute_GetGrabType(outComponents[i]);
				if (grabType != EGrabType::NoGrab) {
					float distToObj = FVector::Dist(ourLocation, outComponents[i]->GetComponentLocation());
					if (distToObj < distanceToGrab || componentToGrab == nullptr) {
						componentToGrab = outComponents[i];
						actorProxy = nullptr;
						distanceToGrab = distToObj;
					}
				}
			} else {
				AActor* componentOwner = outComponents[i]->GetOwner();
				if (componentOwner->Implements<UGrabbable>()) {
					if (IGrabbable::Execute_GetComponentToGrab(componentOwner) == outComponents[i]) {

						EGrabType grabType = IGrabbable::Execute_GetGrabType(componentOwner);
						if (grabType != EGrabType::NoGrab) {

							float distToObj = FVector::Dist(ourLocation, outComponents[i]->GetComponentLocation());
							if (distToObj < distanceToGrab || componentToGrab == nullptr) {
								componentToGrab = outComponents[i];
								actorProxy = componentOwner;
								distanceToGrab = distToObj;
							}
						}
					}
				}
			}

		}
	}
	return componentToGrab;
}

UGrabberComponent* UGrabberComponent::GetOtherGrabber() {
	AActor* owner = GetOwner();
	if (owner) {
		TInlineComponentArray<UGrabberComponent*> grabberComponents;
		owner->GetComponents<UGrabberComponent>(grabberComponents);

		for (UGrabberComponent* grabberComponent : grabberComponents) {
			if (grabberComponent && grabberComponent != this) return grabberComponent;
		}
	}
	return nullptr;
}

void UGrabberComponent::Grab() {
	USceneComponent* componentToGrab = GetComponentToGrab(ActorProxy);
	if (componentToGrab) {
		USceneComponent* actualComponentToGrab = componentToGrab;
		if (!ActorProxy) {
			actualComponentToGrab = IGrabbable::Execute_GetComponentToGrab(componentToGrab);
			if (!actualComponentToGrab) actualComponentToGrab = componentToGrab;
			GrabType = IGrabbable::Execute_GetGrabType(actualComponentToGrab);
		} else {
			GrabType = IGrabbable::Execute_GetGrabType(ActorProxy);
		}

		UGrabberComponent* otherGrabber = GetOtherGrabber();
		if (otherGrabber) {
			if (otherGrabber->GrabbedComponent == actualComponentToGrab) otherGrabber->ReleaseGrab();
		}
		if (ActorProxy) {
			IGrabbable::Execute_GrabStart(ActorProxy, GetHandController());
		} else {
			IGrabbable::Execute_GrabStart(actualComponentToGrab, GetHandController());
		}

		GrabbedComponent = actualComponentToGrab;
		WasGrabbedSimulatingPhysics = false;

		if (GrabType == EGrabType::Free) {
			UPrimitiveComponent* primitiveVersion = Cast<UPrimitiveComponent>(actualComponentToGrab);
			
			if (primitiveVersion) {
				WasGrabbedSimulatingPhysics = primitiveVersion->IsSimulatingPhysics();
			}
			if (WasGrabbedSimulatingPhysics) {

				OldBodyInstance = primitiveVersion->BodyInstance;
				primitiveVersion->SetMassOverrideInKg(NAME_None, 500.0f);
				primitiveVersion->SetLinearDamping(9.0f);
				primitiveVersion->SetAngularDamping(4.5f);
				primitiveVersion->SetMassScale(NAME_None, 1.0f);
				primitiveVersion->SetEnableGravity(false);
				primitiveVersion->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
				PhysicsConstraint->SetConstrainedComponents(this, NAME_None, primitiveVersion, NAME_None);
			} else {
				actualComponentToGrab->AttachToComponent(GetHandController(), FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

void UGrabberComponent::ReleaseGrab() {
	if (GrabbedComponent) {
		if (ActorProxy) {
			IGrabbable::Execute_GrabEnd(ActorProxy, GetHandController());
		} else {
			IGrabbable::Execute_GrabEnd(GrabbedComponent, GetHandController());
		}

		if (GrabType == EGrabType::Free) {
			if (WasGrabbedSimulatingPhysics) {
				PhysicsConstraint->BreakConstraint();
				UPrimitiveComponent* primitiveVersion = Cast<UPrimitiveComponent>(GrabbedComponent);
				if (primitiveVersion) {
					primitiveVersion->SetMassOverrideInKg(NAME_None, OldBodyInstance.GetMassOverride());
					primitiveVersion->SetLinearDamping(OldBodyInstance.LinearDamping);
					primitiveVersion->SetAngularDamping(OldBodyInstance.AngularDamping);
					primitiveVersion->SetMassScale(NAME_None, OldBodyInstance.MassScale);
					primitiveVersion->SetEnableGravity(OldBodyInstance.bEnableGravity);

					primitiveVersion->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, OldBodyInstance.GetResponseToChannel(ECollisionChannel::ECC_Vehicle));
				}
			} else {
				GrabbedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			}
		}
		GrabbedComponent = nullptr;
		ActorProxy = nullptr;
	}
}

void UGrabberComponent::XAxis(float val) {
	if (ActorProxy) {
		IGrabbable::Execute_InputXAxis(ActorProxy, GetHandController(), val);
	} else if (GrabbedComponent){
		IGrabbable::Execute_InputXAxis(GrabbedComponent, GetHandController(), val);
	}
}
void UGrabberComponent::YAxis(float val) {
	if (ActorProxy) {
		IGrabbable::Execute_InputYAxis(ActorProxy, GetHandController(), val);
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputYAxis(GrabbedComponent, GetHandController(), val);
	}
}
void UGrabberComponent::TriggerAxis(float val) {
	if (ActorProxy) {
		IGrabbable::Execute_InputTriggerAxis(ActorProxy, GetHandController(), val);
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputTriggerAxis(GrabbedComponent, GetHandController(), val);
	}
}
void UGrabberComponent::Thumbstick() {
	if (ActorProxy) {
		IGrabbable::Execute_InputThumbstick(ActorProxy, GetHandController());
	}else if (GrabbedComponent){
		IGrabbable::Execute_InputThumbstick(GrabbedComponent, GetHandController());
	}
}
void UGrabberComponent::Trigger() {
	if (ActorProxy) {
		IGrabbable::Execute_InputTrigger(ActorProxy, GetHandController());
	} else if (GrabbedComponent){
		IGrabbable::Execute_InputTrigger(GrabbedComponent, GetHandController());
	}
}
void UGrabberComponent::Button1() {
	if (ActorProxy) {
		IGrabbable::Execute_InputButton1(ActorProxy, GetHandController());
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputButton1(GrabbedComponent, GetHandController());
	}
}
void UGrabberComponent::Button2() {
	if (ActorProxy) {
		IGrabbable::Execute_InputButton2(ActorProxy, GetHandController());
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputButton2(GrabbedComponent, GetHandController());
	}
}