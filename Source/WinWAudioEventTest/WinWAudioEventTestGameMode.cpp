// Copyright Epic Games, Inc. All Rights Reserved.

#include "WinWAudioEventTestGameMode.h"
#include "WinWAudioEventTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

//�~���[�����OVR�R���g���[���X���b�h
#include "HAL/RunnableThread.h"
#include "MirroringVRContorolThread.h"
#include "TimerManager.h"//�x�点�Ȃ��Ɛ���ɓ��삵�Ȃ�����
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
	//�~���[�����OVR�R���g���[���X���b�h�̍폜
	//if (m_mirroringVRContorolThread)
	//{
	//	m_mirroringVRContorolThread->Kill(true);  // �X���b�h���~�itrue �ŏI���܂ő҂j
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
	//�~���[�����OVR�R���g���[���X���b�h�̍폜
	if (m_mirroringVRContorolThread)
	{
		m_mirroringVRContorolThread->Kill(true);  // �X���b�h���~�itrue �ŏI���܂ő҂j
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
	/*�v���C���[�J�������擾*/
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWinWAudioEventTestCharacter::StaticClass(), FoundPlayers);
	
	//�����������p
	AWinWAudioEventTestCharacter* PlayerCharacter = nullptr;

	for (AActor* Actor : FoundPlayers)
	{
	    PlayerCharacter = Cast<AWinWAudioEventTestCharacter>(Actor);
		if (PlayerCharacter) break;
	}
	
	if (PlayerCharacter)
	{
		//�Đ��f�o�C�X��ݒ�
		//bool success = AutoOculusOrMetaSoundDeviceSet::SelectAudioDeviceWithMetaOrOculus();
		//if (success) {
		//	UE_LOG(LogTemp, Log, TEXT("Audio device selected successfully."));
		//}
		//else {
		//	UE_LOG(LogTemp, Warning, TEXT("Failed to select an audio device."));
		//}

		//�X���b�h�̍쐬
		m_mirroringVRContorolRunnable = new MirroringVRContorolThread(PlayerCharacter);
		m_mirroringVRContorolThread = FRunnableThread::Create(m_mirroringVRContorolRunnable, TEXT("MirroringVRContorolThread"));
	}
	
}
