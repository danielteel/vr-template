// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleConstraint.h"


TMap<USimpleConstraint*, UPrimitiveComponent*> USimpleConstraint::ConstraintChild;
TMap<USimpleConstraint*, UPrimitiveComponent*> USimpleConstraint::ConstraintOwner;

bool USimpleConstraint::IsComponentViolated(UPrimitiveComponent* component) {
	TArray<USimpleConstraint*> constraints;
	for (auto& elem : ConstraintChild) {
		if (elem.Value == component) constraints.Add(elem.Key);
	}

	for (auto& elem : constraints) {
		if (elem && elem->IsViolated()) {
			return true;
		} else {
			//Probably dont need recursion, but the options here if I want it
			//if (IsComponentViolated(elem->Owner)) return true;
		}
	}
	return false;
}

USimpleConstraint* USimpleConstraint::MakeConstraint(UPrimitiveComponent* owner, FName ownerSocket, UPrimitiveComponent* child, FName childSocket, float distanceAllowed=0.0f) {
	if (!owner || !child) return nullptr;

	child->AddWorldOffset(owner->GetSocketLocation(ownerSocket) - child->GetSocketLocation(childSocket));

	USimpleConstraint* constraint = NewObject<USimpleConstraint>(owner);
	if (!constraint) return nullptr;

	constraint->AttachToComponent(owner, FAttachmentTransformRules::KeepRelativeTransform);
	constraint->SetWorldLocation(owner->GetSocketLocation(ownerSocket));
	FConstraintInstance ConstraintInstance;
	ConstraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Limited;
	ConstraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Limited;
	ConstraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Limited;
	ConstraintInstance.ProfileInstance.LinearLimit.Limit = distanceAllowed;
	ConstraintInstance.ProfileInstance.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Free;
	ConstraintInstance.ProfileInstance.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Free;
	ConstraintInstance.ProfileInstance.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Free;
	ConstraintInstance.ProfileInstance.bDisableCollision = true;
	ConstraintInstance.ProfileInstance.bEnableProjection = true;
	constraint->ConstraintInstance = ConstraintInstance;
	constraint->InitComponentConstraint();
	constraint->SetConstrainedComponents(owner, NAME_None, child, NAME_None);

	constraint->OwnerSocket = ownerSocket;
	constraint->ChildSocket = childSocket;
	constraint->Owner = owner;
	constraint->Child = child;
	constraint->DistanceAllowed = distanceAllowed;

	constraint->RegisterComponent();

	ConstraintChild.Add(constraint, child);
	ConstraintOwner.Add(constraint, owner);

	return constraint;
}

USimpleConstraint::USimpleConstraint() {

}

void USimpleConstraint::Setup(UPrimitiveComponent* owner, FName ownerSocket, UPrimitiveComponent* child, FName childSocket, float distanceAllowed = 0.0f) {
	if (!owner || !child) return;

	child->AddWorldOffset(owner->GetSocketLocation(ownerSocket) - child->GetSocketLocation(childSocket));

	SetWorldLocation(owner->GetSocketLocation(ownerSocket));
	FConstraintInstance constraintInstance;
	constraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Limited;
	constraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Limited;
	constraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Limited;
	constraintInstance.ProfileInstance.LinearLimit.Limit = distanceAllowed;
	constraintInstance.ProfileInstance.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Free;
	constraintInstance.ProfileInstance.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Free;
	constraintInstance.ProfileInstance.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Free;
	constraintInstance.ProfileInstance.bDisableCollision = true;
	constraintInstance.ProfileInstance.bEnableProjection = true;
	ConstraintInstance = constraintInstance;
	InitComponentConstraint();
	SetConstrainedComponents(owner, NAME_None, child, NAME_None);

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

void USimpleConstraint::SetDistanceAllowed(float distanceAllowed) {
	DistanceAllowed = distanceAllowed;
	ConstraintInstance.ProfileInstance.LinearLimit.Limit = distanceAllowed;
	ConstraintInstance.UpdateLinearLimit();
}

bool USimpleConstraint::IsViolated() {
	if (!Owner || !Child) return false;
	if (FVector::Dist(Owner->GetSocketLocation(OwnerSocket), Child->GetSocketLocation(ChildSocket)) > DistanceAllowed + DistanceBeforeViolated) {
		return true;
	}
	return false;
}