#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Grabbable.h"
#include "Hook.generated.h"


UCLASS(meta = (BlueprintSpawnableComponent))
class VRTEMPLATE_API UHook : public UStaticMeshComponent, public IGrabbable
{
	GENERATED_BODY()


public:

	virtual void OnDestroyPhysicsState() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabTick(class UGrabberComponent* hand, float deltaTime);
	virtual void GrabTick_Implementation(class UGrabberComponent* hand, float deltaTime) override;



	UPROPERTY(EditDefaultsOnly, Category = Setup)
	class USoundBase* AttachNoise = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	class USoundBase* DetachNoise = nullptr;


	UPrimitiveComponent* LastSeenOtherGrabbed = nullptr;
	FVector GrabbedDifference;
	float InitialGrabberDistance = 0.0f;
	bool WasJustBroken = false;
	float HookUpDistance = 12.0f;
	float HookBreakDistance = 24.0f;
	float HookBreakResetDistance = 26.0f;

protected:
	UPROPERTY()
	TMap<UPrimitiveComponent*, class UPhysicsConstraintComponent*> ConstraintsComponents;
};
