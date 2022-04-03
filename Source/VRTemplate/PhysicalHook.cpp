// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicalHook.h"
#include "GrabberComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


UPhysicalHook::UPhysicalHook() {

}


void UPhysicalHook::Setup(UStaticMeshComponent* retainer, UStaticMeshComponent* lock) {
	Retainer = retainer;
	Lock = lock;
}

void UPhysicalHook::InputButton1_Implementation(UGrabberComponent* hand) {
	if (Locked) {
		Locked = false;
		//Retainer Rot = FRotator(0,336,0) Loc = FVector(-1.74, 0, 1)
		//Lock Loc = FVector(-1.13, 0, -1.3)
		if (Retainer) {
			Retainer->SetRelativeRotation(FRotator(336, 0, 0));
			Retainer->SetRelativeLocation(FVector(-1.74f, 0, 1.0f));
		}
		if (Lock) Lock->SetRelativeLocation(FVector(-1.13f, 0, -1.3f));


		this->SetAngularDamping(1.0f);
		this->SetLinearDamping(1.0f);
		this->SetMassOverrideInKg(NAME_None, 1.0f);

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
						component->AddWorldOffset(hookLocation - hookableLocation);
						
						UPhysicsConstraintComponent* physicsConstraint = NewObject<UPhysicsConstraintComponent>(this);
						physicsConstraint->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
						physicsConstraint->SetWorldLocation(hookLocation);
						FConstraintInstance ConstraintInstance;
						ConstraintInstance.ProfileInstance.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Locked;
						ConstraintInstance.ProfileInstance.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Locked;
						ConstraintInstance.ProfileInstance.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Locked;
						ConstraintInstance.ProfileInstance.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Free;
						ConstraintInstance.ProfileInstance.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Free;
						ConstraintInstance.ProfileInstance.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Free;
						ConstraintInstance.ProfileInstance.bDisableCollision = true;
						ConstraintInstance.ProfileInstance.bEnableProjection = true;
						physicsConstraint->ConstraintInstance = ConstraintInstance;
						physicsConstraint->InitComponentConstraint();
						physicsConstraint->SetConstrainedComponents(this, NAME_None, component, NAME_None);

						ConstraintsComponents.Add(component, physicsConstraint);

						this->SetAngularDamping(10000.f);
						this->SetMassOverrideInKg(NAME_None, 0.001f);//prevents hook from causing jerkiness on the attached devices, but unfortunately it looks like its swing around all crazy like
					}
				}
			}
		}
	}
}