// Fill out your copyright notice in the Description page of Project Settings.


#include "HandPoseSender.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Sockets.h"

// Sets default values for this component's properties
UHandPoseSender::UHandPoseSender(const FObjectInitializer& ObjInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	PrimaryComponentTick.bCanEverTick = true;
	IpAddress = "127.0.0.1";
	Port = 5005;
	UdpSocket = nullptr;
}


TSharedRef<FInternetAddr> UHandPoseSender::GetInternetAddress()
{
	TSharedRef<FInternetAddr> TargetAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool IsAddressValid = false;
	TargetAddress->SetIp(*IpAddress, IsAddressValid);
	if (!IsAddressValid)
	{
		UE_LOG(LogTemp, Error, TEXT("IpAddress is invalid."));
	}
	TargetAddress->SetPort(Port);
	return TargetAddress;
}

// Called when the game starts
void UHandPoseSender::BeginPlay()
{
	Super::BeginPlay();

	UdpSocket = FTcpSocketBuilder(TEXT("TCP Socket"))
		.AsReusable()
		.AsNonBlocking()
		.WithSendBufferSize(64 * 64)
		.Build();

	TSharedRef<FInternetAddr> TargetAddress = GetInternetAddress();
	if (!TargetAddress.Get().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Address is invalid."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Used Address %s:%d"), *IpAddress, Port);

	UdpSocket->Connect(TargetAddress.Get());
}

void UHandPoseSender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UdpSocket)
	{
		bool CloseAttempt = UdpSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(UdpSocket);
	}
}

void UHandPoseSender::SendPoseJSON(const FString& InPoseJSON)
{
	if (UdpSocket && UdpSocket->GetConnectionState() == SCS_Connected)
	{
		if (InPoseJSON.Len() != 0)
		{
			int32 Sent = 0;
			
			FTCHARToUTF8 Str(*InPoseJSON);
			int32 StrSize = Str.Length();

			UdpSocket->Send((uint8*)(TCHAR_TO_UTF8(*InPoseJSON)), StrSize, Sent);
		}
	}
}

// Called every frame
void UHandPoseSender::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UdpSocket == nullptr || UdpSocket->GetConnectionState() != SCS_Connected)
	{
		return;
	}

	uint32 PendingDataSize = 0;
	if (UdpSocket->HasPendingData(PendingDataSize))
	{
		TArray<uint8> ReceivedBuffer;
		ReceivedBuffer.SetNum(PendingDataSize);
		int32 BytesRead = 0;
		if (UdpSocket->Recv(ReceivedBuffer.GetData(), ReceivedBuffer.Num(), BytesRead))
		{
			UE_LOG(LogTemp, Warning, TEXT("Message of length %d received"), BytesRead);

			uint8* Buffer = ReceivedBuffer.GetData();
			int32 Num = ReceivedBuffer.Num();
			FString Result;
			Result.Empty(Num);
			while (Num)
			{
				Result += TCHAR(*Buffer);

				++Buffer;
				Num--;
			}
			FString Class, Accuracy;

			Result.Split(TEXT(";"), &Class, &Accuracy);

			OnPredictionResult.Broadcast(Class, Accuracy);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
		}
	}
}

