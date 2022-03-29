// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "HandControllerComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRTEMPLATE_API UHandControllerComponent : public UMotionControllerComponent
{
	GENERATED_BODY()

public:
	UHandControllerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Grab();
	void ReleaseGrab();
	void XAxis(float val);
	void YAxis(float val);
	void TriggerAxis(float val);
	void Thumbstick();
	void Trigger();
	void Button1();
	void Button2();


protected:
	UFUNCTION()
	UHandControllerComponent* GetOtherHand();
	
	UFUNCTION()
	class UGrabberComponent * GetGrabberComponent();

	UPROPERTY()
	class UGrabberComponent* CachedGrabberComponent = nullptr;
};
