// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hoist.h"
#include "HoistControlPanel.h"
#include "CV22.generated.h"

UCLASS()
class VRTEMPLATE_API ACV22 : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACV22();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Root = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	class UStaticMeshComponent* Body = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	class URotorComponent* RotorLeft = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	class URotorComponent* RotorRight = nullptr;


	UPROPERTY(EditDefaultsOnly, Category = Setup)
	class UAudioComponent* AircraftNoise = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Movement, BlueprintReadOnly)
	class UCV22MovementComponent* MovementComponent = nullptr;


	UPROPERTY(EditDefaultsOnly, Category = Setup)
	TSubclassOf<AHoist> HoistClass;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	TSubclassOf<AHoistControlPanel> HoistControlPanelClass;

	UPROPERTY(VisibleAnywhere, Category = Setup)
	AHoist* Hoist = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Setup)
	AHoistControlPanel* HoistControlPanel = nullptr;
};
