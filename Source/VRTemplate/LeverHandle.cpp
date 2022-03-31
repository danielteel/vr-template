// Fill out your copyright notice in the Description page of Project Settings.


#include "LeverHandle.h"
#include "GrabberComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Quat.h"
#include "Lever.h"

// Called when the game starts

ULeverHandle::ULeverHandle() {
	PrimaryComponentTick.bCanEverTick = true;
}


void ULeverHandle::Setup(float minPitch, float maxPitch, float initialPitch, ALever* owner) {
	MinPitch = minPitch;
	MaxPitch = maxPitch;
	Owner = owner;

	CurrentPitch = FMath::Clamp(initialPitch, minPitch, maxPitch);
	TargetPitch = CurrentPitch;
	SetRelativeRotation(FRotator(CurrentPitch, 0.0f, 0.0f));
}


void ULeverHandle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float pitchDelta = CurrentPitch - TargetPitch;
	if (!FMath::IsNearlyZero(pitchDelta)) {
		float moveAmount = pitchDelta > 0 ? -PitchAnimSpeed * DeltaTime : PitchAnimSpeed * DeltaTime;

		if (FMath::Abs(moveAmount) > FMath::Abs(pitchDelta)) {
			moveAmount = FMath::Abs(pitchDelta)*FMath::Sign(moveAmount);
		}

		CurrentPitch += moveAmount;

		SetRelativeRotation(FRotator(CurrentPitch, 0.0f, 0.0f));
	}
}

void ULeverHandle::SetTargetPitch(float targetPitch) {
	TargetPitch = targetPitch;
}

USceneComponent* ULeverHandle::GetComponentToGrab_Implementation() {
	return this;
}

EGrabType ULeverHandle::GetGrabType_Implementation() {
	return EGrabType::Custom;
}

void ULeverHandle::GrabStart_Implementation(UGrabberComponent * hand) {
	GrabOffset = GetSocketLocation(FName("Handle")) - hand->GetComponentLocation();
}

void ULeverHandle::GrabEnd_Implementation(UGrabberComponent * hand) {
	if (Owner) {
		Owner->LeverChanging(CurrentPitch, true);
	}
}

void ULeverHandle::GrabTick_Implementation(UGrabberComponent * hand, float deltaTime) {
	FVector inverseTransform = UKismetMathLibrary::InverseTransformLocation(GetAttachParent()->GetComponentTransform(), hand->GetComponentLocation()+GrabOffset);
	FVector unrotatedVector = FRotator(90.f, 0.0f, 0.0f).UnrotateVector(inverseTransform);
	float pitch = FMath::RadiansToDegrees(UKismetMathLibrary::Atan2(unrotatedVector.Z, unrotatedVector.X));
	pitch = FMath::Clamp(pitch, MinPitch, MaxPitch);

	CurrentPitch = pitch;
	SetRelativeRotation(FRotator(pitch, 0.0f, 0.0f));

	if (Owner) {
		Owner->LeverChanging(CurrentPitch, false);
	}

	TargetPitch = pitch;
}