// Fill out your copyright notice in the Description page of Project Settings.


#include "CV22MovementComponent.h"

UCV22MovementComponent::UCV22MovementComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UCV22MovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyMoveForces(DeltaTime);
	UpdateRadAlt();
}

void UCV22MovementComponent::ApplyMoveForces(float deltaTime){
	if (!Aircraft) {
		UE_LOG(LogTemp, Error, TEXT("No aircraft set in cv22movementcomponent"));
		return;
	}

	FRotator acRotation = Aircraft->GetRelativeRotation();
	FVector localVelocity = GetOwner()->GetActorRotation().UnrotateVector(Aircraft->GetComponentVelocity());


	FVector accel = (LastVelocity - localVelocity) / deltaTime;

	FVector hoverForce = GetOwner()->GetActorUpVector() * Aircraft->GetMass() * (-GetWorld()->GetGravityZ());

	Aircraft->AddForce(hoverForce);//Add enough force just to hover
	Aircraft->AddForce(hoverForce*MoveIntent.Z*VerticalMaxMoveLimit);//Apply force to ascend/descend

	Aircraft->AddForce(InPlaneMaxMoveLimit * MoveIntent.X * GetOwner()->GetActorForwardVector());//Move forward and aft
	Aircraft->AddForce(InPlaneMaxMoveLimit * MoveIntent.Y * GetOwner()->GetActorRightVector());//Move left and right

	float desiredPitch = FMath::Clamp(accel.X / DesiredPitchDivider, -1.0f, 1.0f) * MaxPitchRoll;
	float desiredRoll = FMath::Clamp(accel.Y / DesiredPitchDivider, -1.0f, 1.0f) * -MaxPitchRoll;

	float pitchDiff = desiredPitch - acRotation.Pitch;
	float rollDiff = desiredRoll - acRotation.Roll;

	FVector rotVel = Aircraft->GetUpVector() * 30.0f * deltaTime * MoveIntent.W;
	rotVel += Aircraft->GetForwardVector() * 60.f * deltaTime * (-rollDiff / 30.f);
	rotVel += Aircraft->GetRightVector() * 60.0f * deltaTime * (-pitchDiff / 30.f);

	Aircraft->SetPhysicsAngularVelocityInDegrees(rotVel, true);

	LastVelocity = localVelocity;
}


void UCV22MovementComponent::UpdateRadAlt(){
	FHitResult hitResult;
	FVector traceStart = GetOwner()->GetActorLocation();
	FVector traceEnd = traceStart - FVector::UpVector * 50000.0f;
	FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
	queryParams.AddIgnoredActor(GetOwner());
	bool success = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECollisionChannel::ECC_Visibility, queryParams);

	if (success) {
		AGL = hitResult.Distance*0.0328084f;//convert cm to feet
	} else {
		AGL = -1;
	}
}

int32 UCV22MovementComponent::GetAGL() {
	return AGL;
}

void UCV22MovementComponent::Setup(UPrimitiveComponent* aircraft) {
	Aircraft = aircraft;
	Aircraft->SetPhysicsMaxAngularVelocity(30);
	Aircraft->SetLinearDamping(0.4f);
	Aircraft->SetAngularDamping(1.0f);
}

void UCV22MovementComponent::MoveForward(float val) {
	MoveIntent.X = FMath::Clamp(val, -1.0f, 1.0f);
}
void UCV22MovementComponent::MoveRight(float val) {
	MoveIntent.Y = FMath::Clamp(val, -1.0f, 1.0f);
}
void UCV22MovementComponent::MoveUp(float val) {
	MoveIntent.Z = FMath::Clamp(val, -1.0f, 1.0f);
}
void UCV22MovementComponent::YawRight(float val) {
	MoveIntent.W = FMath::Clamp(val, -1.0f, 1.0f);
}