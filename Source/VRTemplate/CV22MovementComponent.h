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

	void ApplyMoveForces(float deltaTime);
	void UpdateRadAlt();

	UPROPERTY(VisibleAnywhere)
	FVector4 MoveIntent = FVector4(0.0f, 0.0f, 0.0f, 0.0f);//X, Y, Z are translation intents, W is yaw intent


	float InPlaneMaxMoveLimit = 10000000.0f;//In newtons(?)
	float VerticalMaxMoveLimit = 1.0f;//Since hovering depends on ac weight, using -1.0 -> 1.0 to scale

	float DesiredPitchDivider = 1000.0f;

	const float MaxPitchRoll = 15.0f;


	UPROPERTY(VisibleAnywhere)
	FVector4 LastVelocity = FVector4(0.0f, 0.0f, 0.0f, 0.0f);

	int32 AGL = 0;
};
