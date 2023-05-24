// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "SnapshotSenderInfo.generated.h"

class IHttpRequest;
class IHttpResponse;

/**
 * 
 */
UCLASS()
class HANDSML_API ASnapshotSenderInfo : public AInfo
{
	GENERATED_BODY()
public:
	ASnapshotSenderInfo(const FObjectInitializer& ObjInitializer);

protected:
	UPROPERTY(EditAnywhere)
		FString Address;

public:

	UFUNCTION(BlueprintCallable, Category = "HTTP")
		void SendSnapshotData(const FString& InSnapshotJson);

	void OnRequestComplete(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response, bool bWasSuccessful);
};
