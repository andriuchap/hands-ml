// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HandsMLFunctionLibrary.generated.h"

enum class EOculusHandType : uint8;

USTRUCT(BlueprintType)
struct FBoneTransformationData {
	GENERATED_BODY()
public:
	FVector WorldLocation;
	FRotator WorldRotation;
	FVector LocalLocation;
	FRotator LocalRotation;

	FBoneTransformationData()
	{
		WorldLocation = FVector::ZeroVector;
		WorldRotation = FRotator::ZeroRotator;
		LocalLocation = FVector::ZeroVector;
		LocalRotation = FRotator::ZeroRotator;
	}

	FBoneTransformationData(
		const FVector& InWorldLocation,
		const FRotator& InWorldRotation,
		const FVector& InLocalLocation,
		const FRotator& InLocalRotation)
	{
		WorldLocation = InWorldLocation;
		WorldRotation = InWorldRotation;
		LocalLocation = InLocalLocation;
		LocalRotation = InLocalRotation;
	}
};

USTRUCT(BlueprintType)
struct FCapturedPose {
	GENERATED_BODY()
public:
	TArray<FBoneTransformationData> BoneTransformation;
	FString ClassLabel;
	EOculusHandType HandType;

	FCapturedPose();

	FCapturedPose(const TArray<FBoneTransformationData> InBoneTransformation,
		const FString& InClassLabel,
		EOculusHandType InType) {
		BoneTransformation = InBoneTransformation;
		ClassLabel = InClassLabel;
		HandType = InType;
	}
};

/**
 * 
 */
UCLASS()
class HANDSML_API UHandsMLFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		static FCapturedPose CapturePose(class UOculusHandComponent* InHand, const FString& InLabel);
	UFUNCTION(BlueprintCallable, meta=(AdvancedDisplay="FileName"))
		static bool SavePoseToFile(const FCapturedPose& InPose, const FString& FileName);

	UFUNCTION(BlueprintPure)
		static FString GetPoseJson(const FCapturedPose& InPose);

	UFUNCTION(BlueprintCallable)
		static void SendPoseThroughSnapshotSender(const FCapturedPose& InPose, class ASnapshotSenderInfo* InSender);

private:
	static FString LocationAndRotationToPoseString(const FVector& InVector, const FRotator& InRotator);
};
