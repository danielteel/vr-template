// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Grabbable.h"
#include "PhysicalHook.generated.h"

/**
 * 
 */
UCLASS()
class VRTEMPLATE_API UPhysicalHook : public UStaticMeshComponent, public IGrabbable
{
	GENERATED_BODY()

public:
	UPhysicalHook();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InputButton1_Implementation(class UGrabberComponent* hand) override;

	void Setup(UStaticMeshComponent* retainer, UStaticMeshComponent* lock);

protected:
	UPROPERTY(VisibleInstanceOnly)
	UStaticMeshComponent* Retainer;
	
	UPROPERTY(VisibleInstanceOnly)
	UStaticMeshComponent* Lock;

	bool Locked = true;

	UPROPERTY()
	TMap<UPrimitiveComponent*, class UPhysicsConstraintComponent*> ConstraintsComponents;

	float MaxHookDistance = 8.0f;
};
