// Fill out your copyright notice in the Description page of Project Settings.


#include "CV22.h"
#include "Components/StaticMeshComponent.h"
#include "RotorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "DrawDebugHelpers.h"
#include "CV22MovementComponent.h"

// Sets default values
ACV22::ACV22() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(FName("Body"));
	Body->SetupAttachment(Root);

	RotorLeft = CreateDefaultSubobject<URotorComponent>(FName("RotorLeft"));
	RotorLeft->SetupAttachment(Body, FName("RotorLeft"));

	RotorRight = CreateDefaultSubobject<URotorComponent>(FName("RotorRight"));
	RotorRight->SetupAttachment(Body, FName("RotorRight"));

	MovementComponent = CreateDefaultSubobject<UCV22MovementComponent>(FName("MovementComponent"));

	AircraftNoise = CreateDefaultSubobject<UAudioComponent>(FName("AircraftNoise"));
	AircraftNoise->SetupAttachment(Body);
	AircraftNoise->bOverrideAttenuation = true;
	AircraftNoise->bAllowSpatialization = true;
	AircraftNoise->AttenuationOverrides.AttenuationShapeExtents = FVector(4000.0f, 0.0f, 0.0f);
	AircraftNoise->AttenuationOverrides.FalloffDistance = 60000.0f;
	AircraftNoise->AttenuationOverrides.DistanceAlgorithm = EAttenuationDistanceModel::NaturalSound;
	AircraftNoise->AttenuationOverrides.dBAttenuationAtMax = -29.0f;
	AircraftNoise->AttenuationOverrides.bEnableOcclusion = true;
	AircraftNoise->AttenuationOverrides.OcclusionLowPassFilterFrequency = 2500.0f;
}

// Called when the game starts or when spawned
void ACV22::BeginPlay() {
	Super::BeginPlay();
	
	MovementComponent->Setup(Body);

	Hoist = GetWorld()->SpawnActor<AHoist>(HoistClass, Body->GetSocketLocation(FName("Hoist")), Body->GetSocketRotation(FName("Hoist")));
	if (Hoist) {
		Hoist->AttachToComponent(Body, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Hoist"));
	}

	HoistControlPanel = GetWorld()->SpawnActor<AHoistControlPanel>(HoistControlPanelClass, Body->GetSocketLocation(FName("HCP")), Body->GetSocketRotation(FName("HCP")));
	if (HoistControlPanel) {
		HoistControlPanel->AttachToComponent(Body, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("HCP"));
	}

	RotorLeft->IsLeftRotor = true;
	Body->SetMassOverrideInKg(NAME_None, 18143.0f, true);
	Body->SetSimulatePhysics(true);
	Body->SetUseCCD(true);
}


// Called every frame
void ACV22::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	Root->SetWorldLocation(Body->GetComponentLocation());
	FRotator bodyYawRotation = Body->GetComponentRotation();
	bodyYawRotation.Pitch = 0;
	bodyYawRotation.Roll = 0;
	Root->SetWorldRotation(bodyYawRotation);
}
