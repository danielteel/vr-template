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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputButton1(class UPrimitiveComponent* hand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputButton2(class UPrimitiveComponent* hand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputThumbstick(class UPrimitiveComponent* hand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputTrigger(class UPrimitiveComponent* hand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputXAxis(class UPrimitiveComponent* hand, float value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputYAxis(class UPrimitiveComponent* hand, float value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void InputTriggerAxis(class UPrimitiveComponent* hand, float value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabTick(class UPrimitiveComponent* hand);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable") void GrabEnd(class UPrimitiveComponent* hand);
};
