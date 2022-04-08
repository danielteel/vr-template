// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "RotorComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRTEMPLATE_API URotorComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	URotorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	bool IsLeftRotor = false;
};
