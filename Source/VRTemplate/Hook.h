#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Grabbable.h"
#include "Hook.generated.h"


UCLASS()
class VRTEMPLATE_API UHook : public UStaticMeshComponent, public IGrabbable
{
	GENERATED_BODY()


public:
	UHook();

	virtual void OnDestroyPhysicsState() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabStart(class UGrabberComponent* hand);
	virtual void GrabStart_Implementation(class UGrabberComponent* hand) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabEnd(class UGrabberComponent* hand);
	virtual void GrabEnd_Implementation(class UGrabberComponent* hand) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabTick(class UGrabberComponent* hand, float deltaTime);
	virtual void GrabTick_Implementation(class UGrabberComponent* hand, float deltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputButton1(class UGrabberComponent* hand);
	virtual void InputButton1_Implementation(class UGrabberComponent* hand) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputButton2(class UGrabberComponent* hand);
	virtual void InputButton2_Implementation(class UGrabberComponent* hand) override;


	UPrimitiveComponent* LastSeenOtherGrabbed = nullptr;
	FVector GrabbedDifference;
	bool WasJustBroken = false;
	float HookUpDistance = 12.0f;
	float HookBreakDistance = 24.0f;
	float HookBreakResetDistance = 26.0f;

protected:
	UPROPERTY()
	TMap<UPrimitiveComponent*, class UPhysicsConstraintComponent*> ConstraintsComponents;
};
