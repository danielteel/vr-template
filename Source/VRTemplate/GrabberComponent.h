// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Grabbable.h"
#include "GrabberComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRTEMPLATE_API UGrabberComponent : public USphereComponent
{

	GENERATED_BODY()

public:
	UGrabberComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UPROPERTY(VisibleAnywhere)
	class UPhysicsConstraintComponent* PhysicsConstraint = nullptr;

	class UPrimitiveComponent * GetHandController();
	class USceneComponent* GetGrabbedComponent() { return GrabbedComponent; }
	class UPrimitiveComponent* GetGrabbedPrimitiveComponent() { return Cast<UPrimitiveComponent>(GrabbedComponent); }
	class UPrimitiveComponent* GetComponentToGrab(AActor*& actorProxy);

	UGrabberComponent * GetOtherGrabber();

	void Grab();
	void ReleaseGrab();
	void XAxis(float val);
	void YAxis(float val);
	void TriggerAxis(float val);
	void Thumbstick();
	void Trigger();
	void Button1();
	void Button2();

	UPROPERTY()
	AActor* ActorProxy = nullptr;

	UPROPERTY()
	USceneComponent* GrabbedComponent = nullptr;


	bool WasGrabbedSimulatingPhysics = false;
	FBodyInstance OldBodyInstance;
	EGrabType GrabType;
	float GrabDistance = 10.0f;
};
