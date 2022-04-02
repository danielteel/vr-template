// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicalHook.h"
#include "GrabberComponent.h"


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
	}else {
		Locked = true;
		if (Retainer) {
			Retainer->SetRelativeRotation(FRotator(0));
			Retainer->SetRelativeLocation(FVector(0));
		}
		if (Lock) Lock->SetRelativeLocation(FVector(0,0,0));

	}
}