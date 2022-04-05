// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicalHook.h"
#include "GrabberComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SimpleConstraint.h"


UPhysicalHook::UPhysicalHook() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhysicalHook::BeginPlay() {
	Super::BeginPlay();
	SetSimulatePhysics(true);
	SetAngularDamping(0.5f);
	SetLinearDamping(0.05f);
	SetMassOverrideInKg(NAME_None, 1.0f);
	SetUseCCD(true);
}

void UPhysicalHook::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPhysicalHook::Setup(UStaticMeshComponent* retainer, UStaticMeshComponent* lock) {
	Retainer = retainer;
	Lock = lock;
}

void UPhysicalHook::InputButton1_Implementation(UGrabberComponent* hand) {
	if (Locked) {
		Locked = false;
		if (Retainer) {
			Retainer->SetRelativeRotation(FRotator(336, 0, 0));
			Retainer->SetRelativeLocation(FVector(-1.74f, 0, 1.0f));
		}
		if (Lock) Lock->SetRelativeLocation(FVector(-1.13f, 0, -1.3f));


		SetAngularDamping(0.5f);
		SetLinearDamping(0.05f);

		//Break all them constraints
		for (const TPair<UPrimitiveComponent*, UPhysicsConstraintComponent*>& pair : ConstraintsComponents) {
			if (pair.Value) {
				pair.Value->BreakConstraint();
				pair.Value->DestroyComponent();
			}
		}
		ConstraintsComponents.Reset();

	}else {
		Locked = true;
		if (Retainer) {
			Retainer->SetRelativeRotation(FRotator(0));
			Retainer->SetRelativeLocation(FVector(0));
		}
		if (Lock) Lock->SetRelativeLocation(FVector(0,0,0));

		//Make them constraints
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		TArray<AActor*> actorsToIgnore;
		TArray<UPrimitiveComponent*> outComponents;

		FVector hookLocation = GetSocketLocation(FName("Hook"));
		bool result = UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), hookLocation, MaxHookDistance, objectTypes, nullptr, actorsToIgnore, outComponents);
		if (result) {
			TArray<UPrimitiveComponent*> onesThatAreHookable;

			for (UPrimitiveComponent* component : outComponents) {
				if (component->DoesSocketExist(FName("Hookable"))) {
					FVector hookableLocation = component->GetSocketLocation(FName("Hookable"));
					
					if (FVector::Dist(hookLocation, hookableLocation) <= MaxHookDistance) {
						ConstraintsComponents.Add(component, USimpleConstraint::MakeConstraint(this, FName("Hook"), component, FName("Hookable"), 0.0f));

						SetAngularDamping(1.0f);
						SetLinearDamping(0.1f);
					}
				}
			}
		}
	}
}