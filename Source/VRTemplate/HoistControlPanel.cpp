// Fill out your copyright notice in the Description page of Project Settings.


#include "HoistControlPanel.h"
#include "Hoist.h"


// Sets default values
AHoistControlPanel::AHoistControlPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PanelMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("Panel"));
	SetRootComponent(PanelMeshComponent);
	PanelMeshComponent->SetStaticMesh(PanelMesh);

	JettisonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("Jettison"));
	JettisonMeshComponent->SetupAttachment(PanelMeshComponent, FName("Jettison"));
	JettisonMeshComponent->SetStaticMesh(JettisonMesh);

	PowerSwitchMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("OnOff"));
	PowerSwitchMeshComponent->SetupAttachment(PanelMeshComponent, FName("OnOff"));
	PowerSwitchMeshComponent->SetStaticMesh(PowerSwitchMesh);

	UpDownMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("UpDown"));
	UpDownMeshComponent->SetupAttachment(PanelMeshComponent, FName("UpDown"));
	UpDownMeshComponent->SetStaticMesh(UpDownMesh);
}

// Called when the game starts or when spawned
void AHoistControlPanel::BeginPlay()
{
	Super::BeginPlay();

	PowerSwitchMeshComponent->SetVisibility(false);
}


AHoist* AHoistControlPanel::GetHoist() {
	AActor* owner = GetAttachParentActor();

	if (owner) {
		TArray<AActor*> actors;
		owner->GetAttachedActors(actors, true);

		for (auto actor : actors) {
			AHoist* hoist = Cast<AHoist>(actor);
			if (hoist) return hoist;
		}
	}
	return nullptr;
}

// Called every frame
void AHoistControlPanel::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	AHoist* hoist = GetHoist();

	//Check to show power light
	PowerSwitchMeshComponent->SetVisibility(PowerIsOn);

	//Update hoist up/down switch
	float upDownPitch = -60.0f*ExtendCommand;
	FRotator upDownRotation = FRotator(0.0f, 0.0f, upDownPitch);
	UpDownMeshComponent->SetRelativeRotation(upDownRotation);

	//Jettison Switch
	if (JettisonState) {
		JettisonMeshComponent->SetRelativeRotation(FRotator(0, 90, 0));
	} else {
		JettisonMeshComponent->SetRelativeRotation(FRotator(0, 0, 0));
	}

	if (hoist) {
		//Send extend command only if power is on
		if (PowerIsOn) {
			hoist->SetExtendCommand(ExtendCommand);
		} else {
			hoist->SetExtendCommand(0.0f);
		}

		//Set jettison switch state
		if (JettisonState) hoist->Jettison();
	}
}

void AHoistControlPanel::SetPowerState(bool powerState) {
	PowerIsOn = powerState;
}

bool AHoistControlPanel::GetPowerState() {
	return PowerIsOn;
}

void AHoistControlPanel::SetExtendCommand(float extend) {
	ExtendCommand = FMath::Clamp(extend, -1.0f, 1.0f);
}

void AHoistControlPanel::SetJettison(bool jettison) {
	JettisonState = jettison;
	AHoist* hoist = GetHoist();
	if (hoist && jettison) hoist->Jettison();
}


