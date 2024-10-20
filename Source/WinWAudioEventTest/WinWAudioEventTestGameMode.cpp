// Copyright Epic Games, Inc. All Rights Reserved.

#include "WinWAudioEventTestGameMode.h"
#include "WinWAudioEventTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

//ミラーリングVRコントロールスレッド
#include "HAL/RunnableThread.h"
#include "MirroringVRContorolThread.h"
#include "TimerManager.h"//遅らせないと正常に動作しないため
//#include "AutoOculusOrMetaSoundDeviceSet.h"


AWinWAudioEventTestGameMode::AWinWAudioEventTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}

AWinWAudioEventTestGameMode::~AWinWAudioEventTestGameMode()
{
	//ミラーリングVRコントロールスレッドの削除
	//if (m_mirroringVRContorolThread)
	//{
	//	m_mirroringVRContorolThread->Kill(true);  // スレッドを停止（true で終了まで待つ）
	//	delete m_mirroringVRContorolThread;
	//	m_mirroringVRContorolThread = nullptr;
	//}

	//if (m_mirroringVRContorolRunnable)
	//{
	//	delete m_mirroringVRContorolRunnable;
	//	m_mirroringVRContorolRunnable = nullptr;
	//}
}

void AWinWAudioEventTestGameMode::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AWinWAudioEventTestGameMode::StartMirroringVRControl, 1.0f, false);
}

void AWinWAudioEventTestGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//ミラーリングVRコントロールスレッドの削除
	if (m_mirroringVRContorolThread)
	{
		m_mirroringVRContorolThread->Kill(true);  // スレッドを停止（true で終了まで待つ）
		delete m_mirroringVRContorolThread;
		m_mirroringVRContorolThread = nullptr;
	}

	if (m_mirroringVRContorolRunnable)
	{
		delete m_mirroringVRContorolRunnable;
		m_mirroringVRContorolRunnable = nullptr;
	}
}

void AWinWAudioEventTestGameMode::StartMirroringVRControl()
{
	/*プレイヤーカメラを取得*/
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWinWAudioEventTestCharacter::StaticClass(), FoundPlayers);
	
	//見つかった時用
	AWinWAudioEventTestCharacter* PlayerCharacter = nullptr;

	for (AActor* Actor : FoundPlayers)
	{
	    PlayerCharacter = Cast<AWinWAudioEventTestCharacter>(Actor);
		if (PlayerCharacter) break;
	}
	
	if (PlayerCharacter)
	{
		//再生デバイスを設定
		//bool success = AutoOculusOrMetaSoundDeviceSet::SelectAudioDeviceWithMetaOrOculus();
		//if (success) {
		//	UE_LOG(LogTemp, Log, TEXT("Audio device selected successfully."));
		//}
		//else {
		//	UE_LOG(LogTemp, Warning, TEXT("Failed to select an audio device."));
		//}

		//スレッドの作成
		m_mirroringVRContorolRunnable = new MirroringVRContorolThread(PlayerCharacter);
		m_mirroringVRContorolThread = FRunnableThread::Create(m_mirroringVRContorolRunnable, TEXT("MirroringVRContorolThread"));
	}
	
}
