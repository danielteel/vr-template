#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Grabbable.generated.h"


UINTERFACE(MinimalAPI)
class UGrabbable : public UInterface {
	GENERATED_BODY()
};


class VRTEMPLATE_API IGrabbable {
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") USceneComponent* GetComponentToGrab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabStart(class UPrimitiveComponent* hand);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabInputTriggerAction(class UPrimitiveComponent* hand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabInputXAxis(class UPrimitiveComponent* hand, float value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabInputYAxis(class UPrimitiveComponent* hand, float value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabInputTriggerAxis(class UPrimitiveComponent* hand, float value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabTick(class UPrimitiveComponent* hand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabEnd(class UPrimitiveComponent* hand);
};
