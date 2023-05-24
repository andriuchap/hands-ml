// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsMLFunctionLibrary.h"
#include "OculusHandComponent.h"
#include "OculusInputFunctionLibrary.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "SnapshotSenderInfo.h"

FCapturedPose UHandsMLFunctionLibrary::CapturePose(UOculusHandComponent* InHand, const FString& InLabel)
{
	FCapturedPose Result;
	Result.ClassLabel = InLabel;
	Result.HandType = InHand->MeshType;
	TArray<FBoneTransformationData> BoneTransformation;
	for (int i = 0; i < (int)EBone::Bone_Max; i++)
	{
		FString BoneNameStr = UOculusInputFunctionLibrary::GetBoneName((EBone)i);
		FName BoneName = FName(BoneNameStr);

		FVector BoneLocationWorld = InHand->GetBoneLocationByName(BoneName, EBoneSpaces::WorldSpace);
		FRotator BoneRotationWorld = InHand->GetBoneRotationByName(BoneName, EBoneSpaces::WorldSpace);
		FVector BoneLocationComp = InHand->GetBoneLocationByName(BoneName, EBoneSpaces::ComponentSpace);
		FRotator BoneRotationComp = InHand->GetBoneRotationByName(BoneName, EBoneSpaces::ComponentSpace);

		BoneTransformation.Add(
			FBoneTransformationData(
				BoneLocationWorld,
				BoneRotationWorld,
				BoneLocationComp,
				BoneRotationComp
				)
		);
	}

	return Result;
}

bool UHandsMLFunctionLibrary::SavePoseToFile(const FCapturedPose& InPose, const FString& FileName=TEXT(""))
{
	FString PoseJson = GetPoseJson(InPose);

	FString FilePath;

	if (FileName.IsEmpty())
	{
		FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("hands_ml_dataset"), FString::Printf(TEXT("%lld.json"), FDateTime::UtcNow().GetTicks()));
	}
	else
	{
		FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("hands_ml_dataset"), FString::Printf(TEXT("%s.json"), *FileName));
	}

	UE_LOG(LogTemp, Warning, TEXT("Saving result to %s"), *FilePath);

	return FFileHelper::SaveStringToFile(PoseJson, *FilePath);
}

FString UHandsMLFunctionLibrary::GetPoseJson(const FCapturedPose& InPose)
{
	FString ResultString = FString::Printf(TEXT("{\"class\":\"%s\",\"hand\":\"%s\",\"pose\":["), *InPose.ClassLabel, ((InPose.HandType == EOculusHandType::HandLeft) ? TEXT("left") : TEXT("right")));

	for (int i = 0; i < InPose.BoneTransformation.Num(); i++)
	{
		FString BoneNameStr = UOculusInputFunctionLibrary::GetBoneName((EBone)i);
		FName BoneName = FName(BoneNameStr);

		FBoneTransformationData TransformationData = InPose.BoneTransformation[i];

		ResultString.Append(FString::Printf(TEXT("{\"bone\":\"%s\",\"world\":{%s},\"comp\":{%s}}"),
			*BoneNameStr,
			*LocationAndRotationToPoseString(TransformationData.WorldLocation, TransformationData.WorldRotation),
			*LocationAndRotationToPoseString(TransformationData.LocalLocation, TransformationData.LocalRotation)));
		if (i + 1 < InPose.BoneTransformation.Num())
		{
			ResultString.Append(",");
		}
	}
	ResultString.Append(TEXT("]}"));
	return ResultString;
}

void UHandsMLFunctionLibrary::SendPoseThroughSnapshotSender(const FCapturedPose& InPose, ASnapshotSenderInfo* InSender)
{
	if (InSender == nullptr)
	{
		return;
	}

	InSender->SendSnapshotData(GetPoseJson(InPose));
}

FString UHandsMLFunctionLibrary::LocationAndRotationToPoseString(const FVector& InVector, const FRotator& InRotator)
{
	return FString::Printf(TEXT("{\"loc\":{\"x\":%f,\"y\":%f,\"z\":%f},\"rot\":{\"roll\":%f,\"pitch\":%f,\"yaw\":%f}}"), InVector.X, InVector.Y, InVector.Z, InRotator.Roll, InRotator.Pitch, InRotator.Yaw);
}

FCapturedPose::FCapturedPose()
{
	BoneTransformation = TArray<FBoneTransformationData>();
	ClassLabel = TEXT("");
	HandType = EOculusHandType::None;
}
