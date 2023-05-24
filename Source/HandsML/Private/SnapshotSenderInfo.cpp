// Fill out your copyright notice in the Description page of Project Settings.


#include "SnapshotSenderInfo.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

ASnapshotSenderInfo::ASnapshotSenderInfo(const FObjectInitializer& ObjInitializer)
{
	Address = "https://vrlab.lt/hands_ml";
}

void ASnapshotSenderInfo::SendSnapshotData(const FString& InSnapshotJson)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetURL(*Address);
	HttpRequest->SetContentAsString(InSnapshotJson);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ASnapshotSenderInfo::OnRequestComplete);
	HttpRequest->ProcessRequest();
}

void ASnapshotSenderInfo::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	IHttpResponse* HttpResponse = Response.Get();

	
	UE_LOG(LogTemp, Warning, TEXT("%d"), HttpResponse->GetResponseCode());

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Post success"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Something went wrong"));
	}
}
