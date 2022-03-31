#include "Lever.h"
#include "LeverHandle.h"


ALever::ALever(){
	PrimaryActorTick.bCanEverTick = true;


	Base = CreateDefaultSubobject<UStaticMeshComponent>(FName("Base"));
	SetRootComponent(Base);

	Handle = CreateDefaultSubobject<ULeverHandle>(FName("Handle"));
	Handle->SetupAttachment(Base);

}

void ALever::PreRegisterAllComponents() {
	Super::PreRegisterAllComponents();
	Base->SetStaticMesh(BaseMesh);
	Handle->SetStaticMesh(LeverHandleMesh);
	float leverAlpha = float(CurrentValue) / (float(Positions) - 1);
	Handle->Setup(MinPitch, MaxPitch, FMath::Lerp(MinPitch, MaxPitch, leverAlpha), this);
}

void ALever::BeginPlay()
{
	Super::BeginPlay();

	OnLeverChanging.Broadcast(CurrentValue);
	OnLeverChanged.Broadcast(CurrentValue);
}

void ALever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALever::SetLeverValue(int value) {
	if (value == CurrentValue) return;
	value = FMath::Clamp(value, 0, Positions - 1);
	CurrentValue = value;
	float leverAlpha = float(value) / (float(Positions) - 1);
	Handle->SetTargetPitch(FMath::Lerp(MinPitch, MaxPitch, leverAlpha));
	OnLeverChanging.Broadcast(CurrentValue);
	OnLeverChanged.Broadcast(CurrentValue);
}


void ALever::LeverChanging(float pitch, bool endOfChanging) {
	float delta = MaxPitch - MinPitch;
	float value = 1.0f - (MaxPitch - pitch) / delta;
	float divider = delta / float(Positions);
	int step = FMath::Clamp(int((value * delta) / divider), 0, Positions - 1);

	float newTargetPitch = FMath::Lerp(MinPitch, MaxPitch, float(step) / (float(Positions) - 1));

	Handle->SetTargetPitch(newTargetPitch);
	if (CurrentValue != step || endOfChanging) {
		CurrentValue = step;
		OnLeverChanging.Broadcast(CurrentValue);
		if (endOfChanging) {
			OnLeverChanged.Broadcast(CurrentValue);
		}
	}
}