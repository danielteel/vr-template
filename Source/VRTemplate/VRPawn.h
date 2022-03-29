// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HandControllerComponent.h"
#include "VRPawn.generated.h"

UCLASS()
class VRTEMPLATE_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AVRPawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	UHandControllerComponent* GetRightController() { return RightController; }

	UFUNCTION(BlueprintCallable)
	UHandControllerComponent* GetLeftController() { return LeftController; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Root;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHandControllerComponent* LeftController;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHandControllerComponent* RightController;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UGrabberComponent* LeftGrabber;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UGrabberComponent* RightGrabber;


protected:
	virtual void BeginPlay() override;

	float GetVRResetYaw();

	UPROPERTY()
	AActor* CurrentPossessedActor = nullptr;

	//Non-overrideable input actions
	void LeftHandGrab();
	void LeftHandRelease();
	void RightHandGrab();
	void RightHandRelease();
	void LeftHandMenuButton();

	//Overrideable input actions
	virtual void GamepadLeftXAxis(float val);
	virtual void GamepadLeftYAxis(float val);
	virtual void GamepadRightXAxis(float val);
	virtual void GamepadRightYAxis(float val);
	virtual void GamepadRightTriggerAxis(float val);
	virtual void GamepadLeftTriggerAxis(float val);
	virtual void GamepadDPadXAxis(float val);
	virtual void GamepadDPadYAxis(float val);
	virtual void GamepadButtonTop();
	virtual void GamepadButtonRight();
	virtual void GamepadButtonBottom();
	virtual void GamepadButtonLeft();

	virtual void LeftHandXAxis(float val);
	virtual void LeftHandYAxis(float val);
	virtual void LeftHandTriggerAxis(float val);
	virtual void LeftHandThumbstick();
	virtual void LeftHandTrigger();
	virtual void LeftHandXButton();
	virtual void LeftHandYButton();

	virtual void RightHandXAxis(float val);
	virtual void RightHandYAxis(float val);
	virtual void RightHandTriggerAxis(float val);
	virtual void RightHandThumbstick();
	virtual void RightHandTrigger();
	virtual void RightHandAButton();
	virtual void RightHandBButton();
};