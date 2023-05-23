// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HandsMLFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HANDSML_API UHandsMLFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(AdvancedDisplay="FileName"))
		static bool SavePoseToFile(class UOculusHandComponent* InHand, const FString& InLabel, const FString& FileName);

private:
	static FString LocationAndRotationToPoseString(const FVector& InVector, const FRotator& InRotator);
};
