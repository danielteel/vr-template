// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoistControlPanel.generated.h"

UCLASS()
class VRTEMPLATE_API AHoistControlPanel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHoistControlPanel();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetPowerState(bool power);

	UFUNCTION(BlueprintCallable)
	bool GetPowerState();

	UFUNCTION(BlueprintCallable)
	void SetExtendCommand(float extend);

	UFUNCTION(BlueprintCallable)
	void SetJettison(bool jettison);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
	class AHoist* GetHoist();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool JettisonState = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PowerIsOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ExtendCommand = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* PanelMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* JettisonMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* PowerSwitchMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* UpDownMesh = nullptr;

	UPROPERTY()
	class UStaticMeshComponent* PanelMeshComponent = nullptr;

	UPROPERTY()
	class UStaticMeshComponent* JettisonMeshComponent = nullptr;

	UPROPERTY()
	class UStaticMeshComponent* PowerSwitchMeshComponent = nullptr;

	UPROPERTY()
	class UStaticMeshComponent* UpDownMeshComponent = nullptr;
};