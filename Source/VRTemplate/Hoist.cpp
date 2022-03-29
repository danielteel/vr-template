// Fill out your copyright notice in the Description page of Project Settings.


#include "Hoist.h"
#include "Components/StaticMeshComponent.h"
#include "CableComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

void AHoist::SetupCable(UCableComponent* cable, USceneComponent* component1, USceneComponent* component2, UMaterial* material, float width, int32 segments, float length) {
	cable->AttachToComponent(component1, FAttachmentTransformRules::KeepRelativeTransform);
	cable->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
	cable->CableLength = length;
	cable->NumSegments = segments;
	cable->bEnableStiffness = true;
	cable->SolverIterations = 32;
	cable->SetEnableGravity(true);
	cable->bEnableCollision = true;
	cable->EndLocation = FVector(0.0f);
	cable->SetAttachEndToComponent(component2);
	cable->CableWidth = width;
	cable->CollisionFriction = 0.05f;
	cable->SetMaterial(0, material);
}

AHoist::AHoist(){
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostUpdateWork;

	Boomhead = CreateDefaultSubobject<UStaticMeshComponent>(FName("Boomhead"));
	SetRootComponent(Boomhead);

	Base = CreateDefaultSubobject<USphereComponent>(FName("Base"));
	Base->SetupAttachment(Boomhead);
	Base->InitSphereRadius(1.f);
	Base->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	Hook = CreateDefaultSubobject<UStaticMeshComponent>(FName("Hook"));
	Hook->SetupAttachment(Base);

	Constraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Constraint"));
	Constraint->SetupAttachment(Base);

	BoomToBase = CreateDefaultSubobject<UCableComponent>(FName("BoomToBase"));
	BoomToBase->SetupAttachment(Boomhead);

	BaseToHook = CreateDefaultSubobject<UCableComponent>(FName("BaseToHook"));
	BaseToHook->SetupAttachment(Base);

}

void AHoist::PreRegisterAllComponents() {
	Super::PreRegisterAllComponents();
	SetupCable(BoomToBase, Boomhead, Base, CableMaterial, 3.0f, 5, 1.0f);
	SetupCable(BaseToHook, Base, Hook, CableMaterial, 3.0f, 50, 1.0f);
	Boomhead->SetStaticMesh(BoomMesh);
	Hook->SetStaticMesh(HookMesh);
}

void AHoist::BeginPlay(){
	Super::BeginPlay();

	Hook->SetAngularDamping(1.0f);
	Hook->SetSimulatePhysics(true);
	Constraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 0.0f);
	Constraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, 0.0f);
	Constraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 0.0f);
	Constraint->ConstraintInstance.ProfileInstance.LinearLimit.bSoftConstraint = 0;
	Constraint->ConstraintInstance.ProfileInstance.LinearLimit.Restitution = 0.0f;
	Constraint->ConstraintInstance.ProfileInstance.bEnableProjection = 0;
	Constraint->SetConstrainedComponents(Base, NAME_None, Hook, NAME_None);
}

void AHoist::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	FixStuckCable();

	if (Jettisoned) return;

	//UpdateCableGrab(DeltaTime);

	float extendDistance;//Determine hoist cable speed, if its within 14ft of begining or end, limit the speed
	if (HoistOutLength > HoistLimitDist && HoistOutLength < (HoistMaxLength - HoistLimitDist)) {
		extendDistance = ExtendCommand * (DeltaTime*HoistMaxSpeed);
	} else {
		extendDistance = ExtendCommand * (DeltaTime*HoistLimitMaxSpeed);
	}

	SetHoistLength(HoistOutLength + extendDistance);
}

void AHoist::SetExtendCommand(float extendCommand) {
	ExtendCommand = extendCommand;
}

bool AHoist::IsWithinLimit() {
	if (HoistOutLength<HoistLimitDist || HoistOutLength>(HoistMaxLength - HoistLimitDist)) {
		return true;
	}
	return false;
}

void AHoist::SetHoistLength(float hoistLength) {
	if (Jettisoned) return;

	Hook->WakeRigidBody();
	float oldHoistLength = HoistOutLength;

	HoistOutLength = FMath::Clamp(hoistLength, 0.0f, HoistMaxLength);

	float hoistOutMinusOffset = HoistOutLength - HoistGrabOffset;

	BaseToHook->CableLength = hoistOutMinusOffset-3.0f;

	if (HoistGrabOffset < 1.0f) {
		BoomToBase->SetVisibility(false);
	} else {
		BoomToBase->SetVisibility(true);
	}

	if (hoistOutMinusOffset < 1.0f) {
		BaseToHook->SetVisibility(false);
	} else {
		BaseToHook->SetVisibility(true);
	}

	if (HoistOutLength < 10.f) {
		BaseToHook->bEnableCollision = false;
	} else {
		BaseToHook->bEnableCollision = true;
	}

	Constraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, hoistOutMinusOffset);
	Constraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, hoistOutMinusOffset);
	Constraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, hoistOutMinusOffset);

}

void AHoist::FixStuckCable() {
	TArray<FVector> cableParticleLocations;
	bool noStuckParticles = true;
	BaseToHook->GetCableParticleLocations(cableParticleLocations);
	if (cableParticleLocations.Num() > 2) {
		float lastDistanceBetween = 0.0f;
		float distanceBetween = 0.0f;
		float particleDeltaSlop = 15.0f;//10 cm variation allowed between particles
		for (int i = 0; i < cableParticleLocations.Num() - 1; i++) {
			distanceBetween = FVector::Dist(cableParticleLocations[i], cableParticleLocations[i + 1]);
			if (i > 0) {
				if (FMath::Abs(distanceBetween - lastDistanceBetween) > particleDeltaSlop) {
					noStuckParticles = false;
					break;
				}
			}
			lastDistanceBetween = distanceBetween;
		}
		if (noStuckParticles) {
			BaseToHook->bEnableCollision = true;
		} else {
			BaseToHook->bEnableCollision = false;
		}
	}
}

bool AHoist::IsJettisoned() {
	return Jettisoned;
}

void AHoist::Jettison() {
	if (Jettisoned) return;
	Jettisoned = true;

	Base->SetSimulatePhysics(true);
	Base->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BaseToHook->bEnableCollision = true;
	BaseToHook->CableForce = FVector::ZeroVector;
	BoomToBase->SetVisibility(false);
}