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
	FVector Location;
	FRotator Rotation;

	FBoneTransformationData()
	{
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
	}

	FBoneTransformationData(
		const FVector& InLocation,
		const FRotator& InRotation)
	{
		Location = InLocation;
		Rotation = InRotation;
	}
};

USTRUCT(BlueprintType)
struct FCapturedPose {
	GENERATED_BODY()
public:
	TArray<FBoneTransformationData> WorldBoneTransformation;
	TArray<FBoneTransformationData> LocalBoneTransformation;
	FString ClassLabel;
	EOculusHandType HandType;

	FCapturedPose();

	FCapturedPose(const TArray<FBoneTransformationData> &InWorldBoneTransformation,
		const TArray<FBoneTransformationData> &InLocalBoneTransformation,
		const FString& InClassLabel,
		EOculusHandType InType) {
		WorldBoneTransformation = InWorldBoneTransformation;
		LocalBoneTransformation = InLocalBoneTransformation;
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

	UFUNCTION(BlueprintPure)
		static FString GetHandPoseJson(UOculusHandComponent* InHand);

	UFUNCTION(BlueprintCallable)
		static void SendPoseThroughSnapshotSender(const FCapturedPose& InPose, class ASnapshotSenderInfo* InSender);

private:
	static FString LocationAndRotationToPoseString(const FVector& InVector, const FRotator& InRotator);
};
