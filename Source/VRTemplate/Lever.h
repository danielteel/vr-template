// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LeverHandle.h"
#include "Components/SceneComponent.h"
#include "Lever.generated.h"


UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeverWasChanged, int, value);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRTEMPLATE_API ULever : public USceneComponent
{
	GENERATED_BODY()

public:
	virtual void OnRegister() override;

	UPROPERTY(BlueprintAssignable)
	FLeverWasChanged OnLeverChanged;

	UPROPERTY(BlueprintAssignable)
	FLeverWasChanged OnLeverChanging;

	void LeverChanging(float pitch, bool endOfChanging);

	UFUNCTION(BlueprintCallable)
	void SetLeverValue(int value);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* LeverHandleMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* BaseMesh = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	ULeverHandle* LeverHandleComponent = nullptr;
	UPROPERTY(VisibleInstanceOnly)
	UStaticMeshComponent* BaseComponent = nullptr;


	UPROPERTY(EditAnywhere, Category = "Setup", BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	int Positions = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int CurrentValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MinPitch = -80;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MaxPitch = 80;
};
