// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsMLFunctionLibrary.h"
#include "OculusHandComponent.h"
#include "OculusFunctionLibrary.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

bool UHandsMLFunctionLibrary::SavePoseToFile(UOculusHandComponent* InHand, const FString& InLabel, const FString& FileName=TEXT(""))
{
	//FString ResultString = "{\"class\":\"" + InLabel + "\",\"hand\":\"" + ((InHand->MeshType == EOculusHandType::HandLeft) ? "left" : "right") + "\",\"pose\":[{";
	FString ResultString = FString::Printf(TEXT("{\"class\":\"%s\",\"hand\":\"%s\",\"pose\":["), *InLabel, ((InHand->MeshType == EOculusHandType::HandLeft) ? TEXT("left") : TEXT("right")));

	for (int i = 0; i < (int)EBone::Bone_Max; i++)
	{
		FString BoneNameStr = UOculusInputFunctionLibrary::GetBoneName((EBone)i);
		FName BoneName = FName(BoneNameStr);

		FVector BoneLocationWorld = InHand->GetBoneLocationByName(BoneName, EBoneSpaces::WorldSpace);
		FRotator BoneRotationWorld = InHand->GetBoneRotationByName(BoneName, EBoneSpaces::WorldSpace);

		FVector BoneLocationComp = InHand->GetBoneLocationByName(BoneName, EBoneSpaces::ComponentSpace);
		FRotator BoneRotationComp = InHand->GetBoneRotationByName(BoneName, EBoneSpaces::ComponentSpace);

		ResultString.Append(FString::Printf(TEXT("{\"bone\":\"%s\",\"world\":{%s},\"comp\":{%s}}"),
			*BoneNameStr,
			*LocationAndRotationToPoseString(BoneLocationWorld, BoneRotationWorld),
			*LocationAndRotationToPoseString(BoneLocationComp, BoneRotationComp)));
		if (i + 1 < (int)EBone::Bone_Max)
		{
			ResultString.Append(",");
		}
	}
	ResultString.Append(TEXT("]}"));

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

	return FFileHelper::SaveStringToFile(ResultString, *FilePath);
}

FString UHandsMLFunctionLibrary::LocationAndRotationToPoseString(const FVector& InVector, const FRotator& InRotator)
{
	return FString::Printf(TEXT("{\"loc\":{\"x\":%f,\"y\":%f,\"z\":%f},\"rot\":{\"roll\":%f,\"pitch\":%f,\"yaw\":%f}}"), InVector.X, InVector.Y, InVector.Z, InRotator.Roll, InRotator.Pitch, InRotator.Yaw);
}
	