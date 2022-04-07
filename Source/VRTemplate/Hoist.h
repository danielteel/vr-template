// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hoist.generated.h"

UCLASS()
class VRTEMPLATE_API AHoist : public AActor
{
	GENERATED_BODY()
	
public:	
	AHoist();

	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	virtual void PreRegisterAllComponents() override;

	void SetupCable(
		class UCableComponent* cable,
		class USceneComponent* component1,
		class USceneComponent* component2,
		class UMaterial* material,
		float width,
		int32 segments,
		float length
	);

	UPROPERTY(EditAnywhere, Category = "Setup")
		class UStaticMesh* BoomMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setup")
		class UStaticMesh* HookMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setup")
		class UStaticMesh* RetainerMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setup")
		class UStaticMesh* LockMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setup")
		class UMaterial* CableMaterial = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class UStaticMeshComponent* Boomhead = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class USphereComponent* Base = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class UPhysicalHook* Hook = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class UStaticMeshComponent* Lock = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class UStaticMeshComponent* Retainer = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class USimpleConstraint* Constraint = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class UCableComponent* BoomToBase = nullptr;

	UPROPERTY(VisibleInstanceOnly)
		class UCableComponent* BaseToHook = nullptr;

private:
	bool IsWithinLimit();
	void SetHoistLength(float hoistLength);
	void FixStuckCable(); 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ExtendCommand = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float HoistOutLength = 0.0f;

	float HoistLimitDist = 426.72f;
	float HoistMaxLength = 7620.0f;
	float HoistMaxSpeed = 152.4f;
	float HoistLimitMaxSpeed = 38.1f;

	float GrabSpeed = 400.0f;//1 ft/second
	bool GrabConditionMet = false;
	float GrabConditionDelta = 1.0f;//when this close, just snap to hand or boom head
	bool LastGrabState = false;
	float HoistGrabOffset = 0.0f;

	bool Jettisoned = false;

public:	
	UFUNCTION(BlueprintCallable)
	void SetExtendCommand(float val);

	UFUNCTION(BlueprintCallable)
	bool IsJettisoned();

	UFUNCTION(BlueprintCallable)
	void Jettison();
};
