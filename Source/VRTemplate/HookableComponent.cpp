#include "HookableComponent.h"
#include "Components/PrimitiveComponent.h"
#include "HookInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

UHookableComponent::UHookableComponent(){
	PrimaryComponentTick.bCanEverTick = true;
}


void UHookableComponent::OnRegister() {
	Super::OnRegister();
	PhysicsHandle = NewObject<UPhysicsHandleComponent>(this, FName("PhysicsHandle"));
	PhysicsHandle->RegisterComponent();
}

void UHookableComponent::BeginPlay(){
	Super::BeginPlay();
}

void UHookableComponent::OnComponentDestroyed(bool bDestroyingHierarchy) {
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	PhysicsHandle->DestroyComponent();
	PhysicsHandle = nullptr;
}


void UHookableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ComponentHookedTo) {
		PhysicsHandle->SetTargetLocation(ComponentHookedTo->GetSocketLocation(FName("Hook")));
	}
}


UPrimitiveComponent* UHookableComponent::GetHookComponent() {
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	TArray<AActor*> actorsToIgnore;
	TArray<UPrimitiveComponent*> outComponents;

	UPrimitiveComponent* componentToHookTo = nullptr;

	FVector ourLocation = GetSocketLocation(FName("Hook"));

	bool result = UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), ourLocation, HookDistance, objectTypes, nullptr, actorsToIgnore, outComponents);
	if (result) {
		float distanceToHook = 0.0f;
		for (int i = 0; i < outComponents.Num(); i++) {
			bool isHookObject = outComponents[i]->Implements<UHookInterface>();
			if (isHookObject) {
				float distToObj = FVector::Dist(ourLocation, outComponents[i]->GetSocketLocation(FName("Hook")));
				if (distToObj < distanceToHook || componentToHookTo == nullptr) {
					componentToHookTo = outComponents[i];
					distanceToHook = distToObj;
				}
			}
		}
	}
	return componentToHookTo;
}

USceneComponent* UHookableComponent::GetComponentToGrab_Implementation() {
	return this;
}

void UHookableComponent::GrabStart_Implementation(UPrimitiveComponent * hand) {
	PhysicsHandle->ReleaseComponent();
	ComponentHookedTo = nullptr;
	SetSimulatePhysics(false);
	AttachToComponent(hand, FAttachmentTransformRules::KeepWorldTransform);
}

void UHookableComponent::GrabEnd_Implementation(UPrimitiveComponent * hand) {
	DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	SetSimulatePhysics(true);
	ComponentHookedTo = GetHookComponent();
	if (ComponentHookedTo) {
		PhysicsHandle->GrabComponentAtLocation(this, NAME_None, GetSocketLocation("Hook"));
	}
}

void UHookableComponent::GrabTick_Implementation(UPrimitiveComponent * hand) {
}