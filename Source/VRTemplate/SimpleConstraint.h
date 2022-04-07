// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "SimpleConstraint.generated.h"

/**
 * 
 */
UCLASS()
class VRTEMPLATE_API USimpleConstraint : public UPhysicsConstraintComponent
{
	GENERATED_BODY()

public:
	static USimpleConstraint* MakeConstraint(UPrimitiveComponent* owner, FName ownerSocket, UPrimitiveComponent* child, FName childSocket, float distanceAllowed);
	static bool IsComponentViolated(UPrimitiveComponent* component, float* howCloseToViolating);
	static TMap<USimpleConstraint*, UPrimitiveComponent*> ConstraintChild;
	static TMap<USimpleConstraint*, UPrimitiveComponent*> ConstraintOwner;
	

	USimpleConstraint();
	virtual void OnUnregister() override;
	void Setup(UPrimitiveComponent* owner, FName ownerSocket, UPrimitiveComponent* child, FName childSocket, float distanceAllowed);

	void SetDistanceAllowed(float distanceAllowed);
	bool IsViolated(float* howMuch);


	class UPrimitiveComponent* Owner=nullptr;
	class UPrimitiveComponent* Child=nullptr;
	FName OwnerSocket;
	FName ChildSocket;

protected:
	float DistanceAllowed = 0.0f;

	float DistanceBeforeViolated = 20.0f;
};