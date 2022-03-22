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


	UPrimitiveComponent* GetComponentToGrab();

	void Grab();
	void ReleaseGrab();

	USceneComponent* GetGrabbedComponent() { return GrabbedComponent; }

protected:
	UPROPERTY(EditAnywhere)
	float GrabDistance = 10.0f;

	UPROPERTY()
	class USceneComponent* GrabbedComponent = nullptr;

	UPROPERTY()
	class USceneComponent* HighlightedComponent = nullptr;

	UFUNCTION()
	UHandControllerComponent* GetOtherHand();
};
