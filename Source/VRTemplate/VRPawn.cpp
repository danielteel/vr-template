// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVRPawn::AVRPawn(){
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);

	VRRoot = CreateDefaultSubobject<USceneComponent>(FName("VRRoot"));
	VRRoot->SetupAttachment(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(VRRoot);

	LeftController = CreateDefaultSubobject<UHandControllerComponent>(FName("LeftHand"));
	LeftController->SetupAttachment(VRRoot);
	LeftController->SetTrackingSource(EControllerHand::Left);

	RightController = CreateDefaultSubobject<UHandControllerComponent>(FName("RightHand"));
	RightController->SetupAttachment(VRRoot);
	RightController->SetTrackingSource(EControllerHand::Right);
}

void AVRPawn::BeginPlay(){
	Super::BeginPlay();
	
}

void AVRPawn::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("GrabLeft"), EInputEvent::IE_Pressed, this, &AVRPawn::LeftHandGrab);
	PlayerInputComponent->BindAction(FName("GrabLeft"), EInputEvent::IE_Released, this, &AVRPawn::LeftHandRelease);

	PlayerInputComponent->BindAction(FName("GrabRight"), EInputEvent::IE_Pressed, this, &AVRPawn::RightHandGrab);
	PlayerInputComponent->BindAction(FName("GrabRight"), EInputEvent::IE_Released, this, &AVRPawn::RightHandRelease);

	PlayerInputComponent->BindAction(FName("LeftHandThumbstick"), EInputEvent::IE_Pressed, this, &AVRPawn::LeftHandThumbstick);
	PlayerInputComponent->BindAction(FName("LeftHandTrigger"), EInputEvent::IE_Pressed, this, &AVRPawn::LeftHandTrigger);
	PlayerInputComponent->BindAction(FName("LeftHandXButton"), EInputEvent::IE_Pressed, this, &AVRPawn::LeftHandXButton);
	PlayerInputComponent->BindAction(FName("LeftHandYButton"), EInputEvent::IE_Pressed, this, &AVRPawn::LeftHandYButton);

	PlayerInputComponent->BindAction(FName("RightHandThumbstick"), EInputEvent::IE_Pressed, this, &AVRPawn::RightHandThumbstick);
	PlayerInputComponent->BindAction(FName("RightHandTrigger"), EInputEvent::IE_Pressed, this, &AVRPawn::RightHandTrigger);
	PlayerInputComponent->BindAction(FName("RightHandAButton"), EInputEvent::IE_Pressed, this, &AVRPawn::RightHandAButton);
	PlayerInputComponent->BindAction(FName("RightHandBButton"), EInputEvent::IE_Pressed, this, &AVRPawn::RightHandBButton);


	PlayerInputComponent->BindAxis(FName("LeftHandXAxis"), this, &AVRPawn::LeftHandXAxis);
	PlayerInputComponent->BindAxis(FName("LeftHandYAxis"), this, &AVRPawn::LeftHandYAxis);
	PlayerInputComponent->BindAxis(FName("LeftHandTriggerAxis"), this, &AVRPawn::LeftHandTriggerAxis);

	PlayerInputComponent->BindAxis(FName("RightHandXAxis"), this, &AVRPawn::RightHandXAxis);
	PlayerInputComponent->BindAxis(FName("RightHandYAxis"), this, &AVRPawn::RightHandYAxis);
	PlayerInputComponent->BindAxis(FName("RightHandTriggerAxis"), this, &AVRPawn::RightHandTriggerAxis);
}


float AVRPawn::GetVRResetYaw() {
	
	float currentVRRootYaw = VRRoot->GetComponentRotation().Yaw;
	if (!CurrentPossessedActor) {
		return 0-currentVRRootYaw;
	}

	TInlineComponentArray<USceneComponent*> sceneComponents;
	CurrentPossessedActor->GetComponents<USceneComponent>(sceneComponents, false);

	for (USceneComponent* sceneComponent : sceneComponents) {
		if (sceneComponent) {
			if (sceneComponent->DoesSocketExist(FName("VRPawn"))) {
				return sceneComponent->GetSocketRotation(FName("VRPawn")).Yaw - currentVRRootYaw;
			}
		}
	}

	return CurrentPossessedActor->GetActorRotation().Yaw - currentVRRootYaw;
}


//Non-overrideable input actions
void AVRPawn::LeftHandGrab() {
	if (LeftController) LeftController->Grab();
}
void AVRPawn::LeftHandRelease() {
	if (LeftController) LeftController->ReleaseGrab();
}
void AVRPawn::RightHandGrab() {
	if (RightController) RightController->Grab();
}
void AVRPawn::RightHandRelease() {
	if (RightController) RightController->ReleaseGrab();
}


void AVRPawn::LeftHandMenuButton() {
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(GetVRResetYaw(), EOrientPositionSelector::OrientationAndPosition);
}



void AVRPawn::LeftHandXAxis(float val) {
	if (LeftController) LeftController->XAxis(val);
}
void AVRPawn::LeftHandYAxis(float val) {
	if (LeftController) LeftController->YAxis(val);
}
void AVRPawn::LeftHandTriggerAxis(float val) {
	if (LeftController) LeftController->TriggerAxis(val);
}
void AVRPawn::LeftHandThumbstick() {
	if (LeftController) LeftController->Thumbstick();
}
void AVRPawn::LeftHandTrigger() {
	if (LeftController) LeftController->Trigger();
}
void AVRPawn::LeftHandXButton() {
	if (LeftController) LeftController->Button1();
}
void AVRPawn::LeftHandYButton() {
	if (LeftController) LeftController->Button2();
}

void AVRPawn::RightHandXAxis(float val) {
	if (RightController) RightController->XAxis(val);
}
void AVRPawn::RightHandYAxis(float val) {
	if (RightController) RightController->YAxis(val);
}
void AVRPawn::RightHandTriggerAxis(float val) {
	if (RightController) RightController->TriggerAxis(val);
}
void AVRPawn::RightHandThumbstick() {
	if (RightController) RightController->Thumbstick();
}
void AVRPawn::RightHandTrigger() {
	if (RightController) RightController->Trigger();
}
void AVRPawn::RightHandAButton() {
	if (RightController) RightController->Button1();
}
void AVRPawn::RightHandBButton() {
	if (RightController) RightController->Button2();
}


void AVRPawn::GamepadLeftXAxis(float val) {}
void AVRPawn::GamepadLeftYAxis(float val) {}
void AVRPawn::GamepadRightXAxis(float val) {}
void AVRPawn::GamepadRightYAxis(float val) {}
void AVRPawn::GamepadRightTriggerAxis(float val) {}
void AVRPawn::GamepadLeftTriggerAxis(float val) {}
void AVRPawn::GamepadDPadXAxis(float val) {}
void AVRPawn::GamepadDPadYAxis(float val) {}
void AVRPawn::GamepadButtonTop() {}
void AVRPawn::GamepadButtonRight() {}
void AVRPawn::GamepadButtonBottom() {}
void AVRPawn::GamepadButtonLeft() {}