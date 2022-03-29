// Fill out your copyright notice in the Description page of Project Settings.


#include "HandControllerComponent.h"
#include "VRPawn.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GrabberComponent.h"


UHandControllerComponent::UHandControllerComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	SetShowDeviceModel(true);
}

void UHandControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UHandControllerComponent* UHandControllerComponent::GetOtherHand() {
	AVRPawn* owner = Cast<AVRPawn>(GetOwner());
	if (owner) {
		TInlineComponentArray<UHandControllerComponent*> handControllers;
		owner->GetComponents<UHandControllerComponent>(handControllers, false);
		for (auto controller : handControllers) {
			if (controller != this) {
				return controller;
			}
		}
	}
	return nullptr;
}

UGrabberComponent* UHandControllerComponent::GetGrabberComponent() {
	if (CachedGrabberComponent) return CachedGrabberComponent;
	TArray<USceneComponent*> children;
	GetChildrenComponents(true, children);
	for (USceneComponent* child : children) {
		UGrabberComponent* grabber = Cast<UGrabberComponent>(child);
		if (grabber) {
			CachedGrabberComponent = grabber;
			return grabber;
		}
	}
	return nullptr;
}

void UHandControllerComponent::Grab() {	
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->Grab();
}

void UHandControllerComponent::ReleaseGrab() {
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->ReleaseGrab();
}

void UHandControllerComponent::XAxis(float val){
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->XAxis(val);
}
void UHandControllerComponent::YAxis(float val){
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->YAxis(val);
}
void UHandControllerComponent::TriggerAxis(float val){
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->TriggerAxis(val);
}
void UHandControllerComponent::Thumbstick(){
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->Thumbstick();
}
void UHandControllerComponent::Trigger(){
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->Trigger();
}
void UHandControllerComponent::Button1(){
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->Button1();
}
void UHandControllerComponent::Button2(){
	UGrabberComponent* grabber = GetGrabberComponent();
	if (grabber) grabber->Button2();
}