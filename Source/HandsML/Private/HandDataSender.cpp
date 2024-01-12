// Fill out your copyright notice in the Description page of Project Settings.


#include "HandDataSender.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Sockets.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"

UHandDataSender::UHandDataSender(const FObjectInitializer& ObjInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	IpAddress = "127.0.0.1";
	Port = 5005;
	UdpSocket = nullptr;
}


TSharedRef<FInternetAddr> UHandDataSender::GetInternetAddress()
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
void UHandDataSender::BeginPlay()
{
	Super::BeginPlay();
	UdpSocket = FTcpSocketBuilder(TEXT("TCP Socket"))
		.AsReusable()
		.AsNonBlocking()
		.WithSendBufferSize(64 * 64)
		.Build();

	//FPlatformProcess::CreateProc(*FPaths::Combine(FPaths::ProjectDir(), "rcv.py"), nullptr, true, false, false, nullptr, 0, nullptr, nullptr);

	TSharedRef<FInternetAddr> TargetAddress = GetInternetAddress();
	if (!TargetAddress.Get().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Address is invalid."));
	}
	UdpSocket->Connect(TargetAddress.Get());
}

void UHandDataSender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UdpSocket)
	{
		bool CloseAttempt = UdpSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(UdpSocket);
	}
}

void UHandDataSender::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UdpSocket == nullptr)
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
			//FString String = BytesToString(ReceivedBuffer.GetData(), ReceivedBuffer.Num());

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

void UHandDataSender::SendHandData(const FCapturedPose& InPose)
{
	if (UdpSocket)
	{
		FString Result = UHandsMLFunctionLibrary::GetPoseJson(InPose);
		TCHAR* Chars = Result.GetCharArray().GetData();
		int32 Size = FCString::Strlen(Chars);

		int32 Sent = 0;
		//TArray<uint8> Buffer;
		//FMemoryWriter ArW(Buffer);

		//FImageUtils::ExportRenderTarget2DAsPNG(InRT, ArW);

		UE_LOG(LogTemp, Warning, TEXT("SENDING: %d"), Size);

		//UdpSocket->Send(Buffer.GetData(), Buffer.Num(), Sent);
		UdpSocket->Send((uint8*)TCHAR_TO_UTF8(Chars), Size, Sent);

		UE_LOG(LogTemp, Warning, TEXT("SENT: %d"), Size);
	}
}