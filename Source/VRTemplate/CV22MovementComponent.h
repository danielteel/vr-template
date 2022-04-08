#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "CV22MovementComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VRTEMPLATE_API UCV22MovementComponent : public UMovementComponent {
	GENERATED_BODY()

public:
	UCV22MovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category=Movement)
	void MoveForward(float val);
	UFUNCTION(BlueprintCallable, Category = Movement)
	void MoveRight(float val);
	UFUNCTION(BlueprintCallable, Category = Movement)
	void MoveUp(float val);
	UFUNCTION(BlueprintCallable, Category = Movement)
	void YawRight(float val);

	UFUNCTION(BlueprintCallable, Category = Movement)
	FVector GetVelocity() { return LastVelocity; }

	UFUNCTION(BlueprintCallable, Category = Movement)
	int32 GetAGL();

	UFUNCTION(BlueprintCallable, Category = Setup)
	void Setup(class UPrimitiveComponent* aircraft);

protected:
	class UPrimitiveComponent* Aircraft = nullptr;

	UPROPERTY(VisibleAnywhere)
	FVector4 MoveIntent = FVector4(0.0f, 0.0f, 0.0f, 0.0f);//X, Y, Z are translation intents, W is yaw intent

	float Roll = 0;
	float Pitch = 0;
	float CurrentRoll = 0;
	float CurrentPitch = 0;

	UPROPERTY(VisibleAnywhere)
	FVector4 LastVelocity = FVector4(0.0f, 0.0f, 0.0f, 0.0f);

	int32 AGL = 0;
};
