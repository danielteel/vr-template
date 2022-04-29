// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleConstraint.h"
#include "Kismet/KismetMathLibrary.h"

TMap<USimpleConstraint*, UPrimitiveComponent*> USimpleConstraint::ConstraintChild;
TMap<USimpleConstraint*, UPrimitiveComponent*> USimpleConstraint::ConstraintOwner;

bool USimpleConstraint::IsComponentViolated(UPrimitiveComponent* component, float* howCloseToViolating = nullptr) {
	if (howCloseToViolating) *howCloseToViolating = 0.0f;

	TArray<USimpleConstraint*> constraints;
	for (auto& elem : ConstraintChild) {
		if (elem.Value == component) constraints.Add(elem.Key);
	}

	for (auto& elem : constraints) {
		if (elem && elem->IsViolated(howCloseToViolating)) {
			return true;//Only return true for children so both controllers dont drop what theyre holding
		}
	}

	constraints.Reset();
	for (auto& elem : ConstraintOwner) {
		if (elem.Value == component) constraints.Add(elem.Key);
	}	
	for (auto& elem : constraints) {
		if (elem) {
			float howClose = 0.0f;
			elem->IsViolated(&howClose);
			if (howClose > *howCloseToViolating) *howCloseToViolating = howClose;
		}
	}

	return false;
}

FConstraintInstance USimpleConstraint::MakeConstraintInstance(float distanceAllowed=0.0f){
	FConstraintInstance constraintInstance;
	if (FMath::IsNearlyZero(distanceAllowed)) {
		constraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Locked;
		constraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Locked;
		constraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Locked;
	} else {
		constraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Limited;
		constraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Limited;
		constraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Limited;
	}
	constraintInstance.ProfileInstance.LinearLimit.Limit = distanceAllowed;
	constraintInstance.ProfileInstance.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Free;
	constraintInstance.ProfileInstance.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Free;
	constraintInstance.ProfileInstance.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Free;
	constraintInstance.ProfileInstance.bDisableCollision = true;
	constraintInstance.ProfileInstance.bEnableProjection = true;
	constraintInstance.ProfileInstance.ProjectionLinearTolerance = 0.1f;
	return constraintInstance;
}


USimpleConstraint* USimpleConstraint::MakeConstraint(UPrimitiveComponent* owner, FName ownerSocket, UPrimitiveComponent* child, FName childSocket, float distanceAllowed=0.0f) {
	if (ensureMsgf(!owner || !child, TEXT("Owner or Child is nullptr"))){
		return nullptr;
	}

	child->AddWorldOffset(owner->GetSocketLocation(ownerSocket) - child->GetSocketLocation(childSocket));

	USimpleConstraint* constraint = NewObject<USimpleConstraint>(owner);
	if (!constraint) return nullptr;
	constraint->AttachToComponent(owner, FAttachmentTransformRules::KeepRelativeTransform);
	constraint->Setup(owner, ownerSocket, child, childSocket, distanceAllowed);
	constraint->RegisterComponent();

	ConstraintChild.Add(constraint, child);
	ConstraintOwner.Add(constraint, owner);

	return constraint;
}

USimpleConstraint::USimpleConstraint() {
}

void USimpleConstraint::Setup(UPrimitiveComponent* owner, FName ownerSocket, UPrimitiveComponent* child, FName childSocket, float distanceAllowed = 0.0f) {
	if (ensureMsgf(!owner || !child, TEXT("Owner or Child is nullptr"))){
		return;
	}

	child->AddWorldOffset(owner->GetSocketLocation(ownerSocket) - child->GetSocketLocation(childSocket));

	SetWorldLocation(owner->GetSocketLocation(ownerSocket));


	ConstraintInstance = MakeConstraintInstance(distanceAllowed);
	InitComponentConstraint();
	SetConstrainedComponents(owner, NAME_None, child, NAME_None);

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	OwnerSocket = ownerSocket;
	ChildSocket = childSocket;
	Owner = owner;
	Child = child;
	DistanceAllowed = distanceAllowed;

	ConstraintChild.Add(this, child);
	ConstraintOwner.Add(this, owner);
}

void USimpleConstraint::OnUnregister() {
	Super::OnUnregister();
	BreakConstraint();
	ConstraintChild.Remove(this);
	ConstraintOwner.Remove(this);
}



void USimpleConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector OwnerLocation = Owner->GetComponentLocation();
	FVector ChildLocation = Child->GetComponentLocation();
	OwnerVelocity = (OwnerLastPosition - OwnerLocation)/DeltaTime;
	ChildLocation = (ChildLastPosition - ChildLocation)/DeltaTime;
	OwnerLastPosition = OwnerLocation;
	ChildLastPosition = ChildLocation;
}

void USimpleConstraint::SetDistanceAllowed(float distanceAllowed) {
	DistanceAllowed = distanceAllowed;
	ConstraintInstance.ProfileInstance.LinearLimit.Limit = distanceAllowed;

	if (FMath::IsNearlyZero(distanceAllowed)) {
		ConstraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Locked;
		ConstraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Locked;
		ConstraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Locked;
	} else {
		ConstraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Limited;
		ConstraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Limited;
		ConstraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Limited;
	}

	ConstraintInstance.UpdateLinearLimit();
}

bool USimpleConstraint::IsViolated(float* howMuch) {
	if (!Owner || !Child) return false;

	FVector ownerDelta = FVector::ZeroVector;
	FVector childDelta = FVector::ZeroVector;
	UWorld* world = GetWorld();
	if (world) {
		float deltaTime = world->DeltaTimeSeconds;
		if (Owner->IsSimulatingPhysics()) ownerDelta = OwnerVelocity*deltaTime;
		if (Child->IsSimulatingPhysics()) childDelta = ChildVelocity*deltaTime;
	}

	float jointDist = FVector::Dist(Owner->GetSocketLocation(OwnerSocket) + ownerDelta, Child->GetSocketLocation(ChildSocket) + childDelta);
	if (jointDist > DistanceAllowed + DistanceBeforeViolated) {
		return true;
	}
	if (howMuch && jointDist > DistanceAllowed+DistanceBeforeViolated/4.0f) {
		*howMuch = (jointDist - DistanceAllowed - DistanceBeforeViolated / 4.0f) / (DistanceBeforeViolated - DistanceBeforeViolated / 4.0f);
	} else if (howMuch) {
		*howMuch = 0.0f;
	}
	return false;
}