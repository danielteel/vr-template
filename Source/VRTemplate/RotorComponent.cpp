// Fill out your copyright notice in the Description page of Project Settings.


#include "RotorComponent.h"


URotorComponent::URotorComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void URotorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AddLocalRotation(FRotator(0.0f, 1900.0f * DeltaTime * (IsLeftRotor ? 1.0f : -1.0f), 0.0f));
}
