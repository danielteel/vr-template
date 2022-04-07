// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabberComponent.h"
#include "VRPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "SimpleConstraint.h"
#include "HandControllerComponent.h"
#include "DrawDebugHelpers.h"

UGrabberComponent::UGrabberComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrabberComponent::BeginPlay() {
	Super::BeginPlay();
}


void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float hapticValue = 0.0f;

	if (GrabbedComponent){
		if (GrabType == EGrabType::Free) {
			UPrimitiveComponent* primitiveVersion = Cast<UPrimitiveComponent>(GrabbedComponent);
			if (primitiveVersion) {
				float howCloseToViolating = 0.0f;
				if (USimpleConstraint::IsComponentViolated(primitiveVersion, &howCloseToViolating)) {
					ReleaseGrab();
					return;
				} else {
					if (!FMath::IsNearlyZero(howCloseToViolating)) {
						//Set Haptics
						hapticValue = howCloseToViolating;
					}
				}
			}
		}
		if (ActorProxy) {
			IGrabbable::Execute_GrabTick(ActorProxy, this, DeltaTime);
		} else {
			IGrabbable::Execute_GrabTick(GrabbedComponent, this, DeltaTime);
		}
	}
	AVRPawn* pawn = Cast<AVRPawn>(GetOwner());
	if (pawn) {
		APlayerController* playerController = Cast<APlayerController>(pawn->GetController());
		if (playerController) {
			playerController->SetHapticsByValue(hapticValue, hapticValue, GetHandController()->GetTrackingSource());
		}
	}
}

UHandControllerComponent* UGrabberComponent::GetHandController() {
	return Cast<UHandControllerComponent>(this->GetAttachParent());
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
			IGrabbable::Execute_GrabStart(ActorProxy, this);
		} else {
			IGrabbable::Execute_GrabStart(actualComponentToGrab, this);
		}

		GrabbedComponent = actualComponentToGrab;

		InitialGrabOffset = GetComponentLocation() - actualComponentToGrab->GetComponentLocation();

		if (GrabType == EGrabType::Free) {
			UPrimitiveComponent* primitiveVersion = Cast<UPrimitiveComponent>(GrabbedComponent);
			if (primitiveVersion) {
				WasSimulatingPhysics = primitiveVersion->IsSimulatingPhysics();
				if (WasSimulatingPhysics) primitiveVersion->SetSimulatePhysics(false);
			} else {
				WasSimulatingPhysics = false;
			}
			actualComponentToGrab->AttachToComponent(GetHandController(), FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void UGrabberComponent::ReleaseGrab() {
	if (GrabbedComponent) {
		if (GrabType == EGrabType::Free) {
			GrabbedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			UPrimitiveComponent* primitiveVersion = Cast<UPrimitiveComponent>(GrabbedComponent);
			if (primitiveVersion && WasSimulatingPhysics) primitiveVersion->SetSimulatePhysics(true);
		}
		if (ActorProxy) {
			IGrabbable::Execute_GrabEnd(ActorProxy, this);
		} else {
			IGrabbable::Execute_GrabEnd(GrabbedComponent, this);
		}
		GrabbedComponent = nullptr;
		ActorProxy = nullptr;
	}
}

void UGrabberComponent::XAxis(float val) {
	if (ActorProxy) {
		IGrabbable::Execute_InputXAxis(ActorProxy, this, val);
	} else if (GrabbedComponent){
		IGrabbable::Execute_InputXAxis(GrabbedComponent, this, val);
	}
}
void UGrabberComponent::YAxis(float val) {
	if (ActorProxy) {
		IGrabbable::Execute_InputYAxis(ActorProxy, this, val);
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputYAxis(GrabbedComponent, this, val);
	}
}
void UGrabberComponent::TriggerAxis(float val) {
	if (ActorProxy) {
		IGrabbable::Execute_InputTriggerAxis(ActorProxy, this, val);
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputTriggerAxis(GrabbedComponent, this, val);
	}
}
void UGrabberComponent::Thumbstick() {
	if (ActorProxy) {
		IGrabbable::Execute_InputThumbstick(ActorProxy, this);
	}else if (GrabbedComponent){
		IGrabbable::Execute_InputThumbstick(GrabbedComponent, this);
	}
}
void UGrabberComponent::Trigger() {
	if (ActorProxy) {
		IGrabbable::Execute_InputTrigger(ActorProxy, this);
	} else if (GrabbedComponent){
		IGrabbable::Execute_InputTrigger(GrabbedComponent, this);
	}
}
void UGrabberComponent::Button1() {
	if (ActorProxy) {
		IGrabbable::Execute_InputButton1(ActorProxy, this);
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputButton1(GrabbedComponent, this);
	}
}
void UGrabberComponent::Button2() {
	if (ActorProxy) {
		IGrabbable::Execute_InputButton2(ActorProxy, this);
	} else if (GrabbedComponent) {
		IGrabbable::Execute_InputButton2(GrabbedComponent, this);
	}
}