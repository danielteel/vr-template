// Fill out your copyright notice in the Description page of Project Settings.


#include "HandControllerComponent.h"
#include "VRPawn.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Grabbable.h"


UHandControllerComponent::UHandControllerComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	SetShowDeviceModel(true);
}

void UHandControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GrabbedComponent) {
		IGrabbable::Execute_GrabTick(GrabbedComponent, this);////update this with new scheme
	}
}

UHandControllerComponent* UHandControllerComponent::GetOtherHand() {
	AVRPawn* owner = Cast<AVRPawn>(GetOwner());
	if (owner) {
		TInlineComponentArray<UHandControllerComponent*> handControllers;
		owner->GetComponents<UHandControllerComponent>(handControllers, false);
		for (auto controller : handControllers) {
			if (controller != this) {
				return Cast<UHandControllerComponent>(controller);
			}
		}
	}
	return nullptr;
}


UPrimitiveComponent* UHandControllerComponent::GetComponentToGrab() {
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	TArray<AActor*> actorsToIgnore;
	TArray<UPrimitiveComponent*> outComponents;

	UPrimitiveComponent* componentToGrab = nullptr;

	FVector handLocation = GetComponentLocation();

	bool result = UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), handLocation, GrabDistance*GetComponentScale().GetMin(), objectTypes, nullptr, actorsToIgnore, outComponents);
	if (result) {
		float distanceToGrab = 0.0f;
		for (int i = 0; i < outComponents.Num(); i++) {
			IGrabbable* grabbableObject = Cast<IGrabbable>(outComponents[i]);
			if (grabbableObject) {

				float distToObj = FVector::Dist(handLocation, outComponents[i]->GetComponentLocation());
				if (distToObj < distanceToGrab || componentToGrab == nullptr) {
					componentToGrab = outComponents[i];
					distanceToGrab = distToObj;
				}
			}
		}
	}
	return componentToGrab;
}


void UHandControllerComponent::Grab() {	
	USceneComponent* componentToGrab = GetComponentToGrab();
	if (componentToGrab) {
		componentToGrab = IGrabbable::Execute_GetComponentToGrab(componentToGrab);

		UHandControllerComponent* otherHand = GetOtherHand();
		if (otherHand) {
			if (otherHand->GrabbedComponent == componentToGrab) otherHand->ReleaseGrab();
		}
		IGrabbable::Execute_GrabStart(componentToGrab, this);
		GrabbedComponent = componentToGrab;
	}
}

void UHandControllerComponent::ReleaseGrab() {
	if (GrabbedComponent) {
		IGrabbable::Execute_GrabEnd(GrabbedComponent, this);
		GrabbedComponent = nullptr;
	}
}