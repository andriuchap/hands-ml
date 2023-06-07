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
	TArray<FBoneTransformationData> WorldBoneTransformation;
	TArray<FBoneTransformationData> LocalBoneTransformation;
	for (int i = 0; i < (int)EBone::Bone_Max; i++)
	{
		FString BoneNameStr = UOculusInputFunctionLibrary::GetBoneName((EBone)i);
		FName BoneName = FName(BoneNameStr);

		FVector BoneLocationWorld = InHand->GetBoneLocationByName(BoneName, EBoneSpaces::WorldSpace);
		FRotator BoneRotationWorld = InHand->GetBoneRotationByName(BoneName, EBoneSpaces::WorldSpace);
		FVector BoneLocationComp = InHand->GetBoneLocationByName(BoneName, EBoneSpaces::ComponentSpace);
		FRotator BoneRotationComp = InHand->GetBoneRotationByName(BoneName, EBoneSpaces::ComponentSpace);

		WorldBoneTransformation.Add(
			FBoneTransformationData(
				BoneLocationWorld,
				BoneRotationWorld
				)
		);
		WorldBoneTransformation.Add(
			FBoneTransformationData(
				BoneLocationComp,
				BoneRotationComp
			)
		);
	}
	Result.WorldBoneTransformation = WorldBoneTransformation;
	Result.LocalBoneTransformation = LocalBoneTransformation;
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

	int Num = InPose.WorldBoneTransformation.Num();

	for (int i = 0; i < Num; i++)
	{
		FString BoneNameStr = UOculusInputFunctionLibrary::GetBoneName((EBone)i);
		FName BoneName = FName(BoneNameStr);

		FBoneTransformationData WorldTransformationData = InPose.WorldBoneTransformation[i];
		FBoneTransformationData LocalTransformationData = InPose.WorldBoneTransformation[i];

		ResultString.Append(FString::Printf(TEXT("{\"bone\":\"%s\",\"world\":%s,\"comp\":%s}"),
			*BoneNameStr,
			*LocationAndRotationToPoseString(WorldTransformationData.Location, WorldTransformationData.Rotation),
			*LocationAndRotationToPoseString(LocalTransformationData.Location, LocalTransformationData.Rotation)));
		if (i + 1 < Num)
		{
			ResultString.Append(",");
		}
	}
	ResultString.Append(TEXT("]}"));
	return ResultString;
}

FString UHandsMLFunctionLibrary::GetHandPoseJson(UOculusHandComponent* InHand)
{
	FString Result = "{\"rotations\":[";
	for (int i = 0; i < (int)EBone::Bone_Max; i++)
	{
		FString BoneNameStr = UOculusInputFunctionLibrary::GetBoneName((EBone)i);
		FName BoneName = FName(BoneNameStr);

		FRotator BoneRotation = InHand->GetBoneRotationByName(BoneName, EBoneSpaces::ComponentSpace);

		Result.Append(FString::Printf(TEXT("{\"roll\":%f,\"pitch\":%f,\"yaw\":%f}"), BoneRotation.Roll, BoneRotation.Pitch, BoneRotation.Yaw));
		if (i < (int)EBone::Pinky_Tip)
		{
			Result.Append(",");
		}
	}

	Result.Append("]}");
	return Result;
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
	WorldBoneTransformation = TArray<FBoneTransformationData>();
	LocalBoneTransformation = TArray<FBoneTransformationData>();
	ClassLabel = TEXT("");
	HandType = EOculusHandType::None;
}
