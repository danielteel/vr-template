// Fill out your copyright notice in the Description page of Project Settings.


#include "Lever.h"


void ULever::OnRegister() {
	Super::OnRegister();

	BaseComponent = NewObject<UStaticMeshComponent>(this, FName("Base"));
	BaseComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	BaseComponent->RegisterComponent();
	BaseComponent->SetStaticMesh(BaseMesh);

	LeverHandleComponent = NewObject<ULeverHandle>(this, FName("Handle"));
	LeverHandleComponent->AttachToComponent(BaseComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	LeverHandleComponent->RegisterComponent();
	LeverHandleComponent->SetStaticMesh(LeverHandleMesh);
	LeverHandleComponent->Setup(MinPitch, MaxPitch, MinPitch, this);
}


void ULever::SetLeverValue(int value) {
	value = FMath::Clamp(value, 0, Positions-1);
	CurrentValue = value;
	float leverAlpha = float(value) / (float(Positions) - 1);
	LeverHandleComponent->SetTargetPitch(FMath::Lerp(MinPitch, MaxPitch, leverAlpha));
	OnLeverChanged.Broadcast(CurrentValue);
}


void ULever::LeverChanging(float pitch, bool endOfChanging) {
	float delta = MaxPitch - MinPitch;
	float value = 1.0f - (MaxPitch - pitch) / delta;
	float divider = delta / float(Positions);
	int step = FMath::Clamp(int((value * delta) / divider), 0, Positions-1);

	float newTargetPitch = FMath::Lerp(MinPitch, MaxPitch, float(step) / (float(Positions) - 1));

	LeverHandleComponent->SetTargetPitch(newTargetPitch);
	if (CurrentValue != step || endOfChanging) {
		CurrentValue = step;
		if (endOfChanging) {
			OnLeverChanged.Broadcast(CurrentValue);
		} else {
			OnLeverChanging.Broadcast(CurrentValue);
		}
	}
}