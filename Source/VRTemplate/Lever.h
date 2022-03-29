#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lever.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLeverWasChanged, int, value);

UCLASS()
class VRTEMPLATE_API ALever : public AActor
{
	GENERATED_BODY()
	
public:	
	ALever();
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void PreRegisterAllComponents() override;
	virtual void BeginPlay() override;
	
	friend class ULeverHandle;
	void SetLeverValue(int value);
	void LeverChanging(float pitch, bool endOfChanging);


	UPROPERTY(VisibleInstanceOnly)
	ULeverHandle* Handle = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	UStaticMeshComponent* Base = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FLeverWasChanged OnLeverChanged;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FLeverWasChanged OnLeverChanging;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* LeverHandleMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* BaseMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int Positions = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int CurrentValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MinPitch = -80;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MaxPitch = 80;
};
