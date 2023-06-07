// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandPoseSender.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPredictionResultDelegate, FString, Class, FString, Accuracy);

UCLASS( ClassGroup=(Custom), Config=Game, meta=(BlueprintSpawnableComponent) )
class HANDSML_API UHandPoseSender : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHandPoseSender(const FObjectInitializer& ObjInitializer);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config)
		FString IpAddress;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config)
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
		void SendPoseJSON(const FString &InPoseJSON);

	UPROPERTY(BlueprintAssignable)
		FPredictionResultDelegate OnPredictionResult;
};
