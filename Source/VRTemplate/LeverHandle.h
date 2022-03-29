// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Grabbable.h"
#include "LeverHandle.generated.h"

/**
 * 
 */
UCLASS()
class VRTEMPLATE_API ULeverHandle : public UStaticMeshComponent, public IGrabbable
{
	GENERATED_BODY()

public:
	ULeverHandle();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetTargetPitch(float pitch);
	void Setup(float minPitch, float maxPitch, float initialPitch, class ALever* owner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") USceneComponent* GetComponentToGrab();
	virtual USceneComponent* GetComponentToGrab_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") EGrabType GetGrabType();
	virtual EGrabType GetGrabType_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabStart(class UPrimitiveComponent* hand);
	virtual void GrabStart_Implementation(class UPrimitiveComponent* hand) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabEnd(class UPrimitiveComponent* hand);
	virtual void GrabEnd_Implementation(class UPrimitiveComponent* hand) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabTick(class UPrimitiveComponent* hand);
	virtual void GrabTick_Implementation(class UPrimitiveComponent* hand) override;

private:
	UPROPERTY()
	class ALever* Owner = nullptr;

	FVector GrabOffset;

	float MinPitch = 0;
	float MaxPitch = 0;

	float CurrentPitch = 0;
	float TargetPitch = 0;
	float PitchAnimSpeed = 500.0f;
};
