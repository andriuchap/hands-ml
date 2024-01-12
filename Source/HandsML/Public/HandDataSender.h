// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandsMLFunctionLibrary.h"
#include "HandDataSender.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPredictionResultDelegate, FString, Class, FString, Accuracy);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HANDSML_API UHandDataSender : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UHandDataSender(const FObjectInitializer& ObjInitializer);

protected:

	UPROPERTY(EditAnywhere)
	FString IpAddress;
	UPROPERTY(EditAnywhere)
	int32 Port;

private:
	class FSocket* UdpSocket;

private:
	TSharedRef<FInternetAddr> GetInternetAddress();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SendHandData(const FCapturedPose& InPose);

	UPROPERTY(BlueprintAssignable)
	FPredictionResultDelegate OnPredictionResult;
};