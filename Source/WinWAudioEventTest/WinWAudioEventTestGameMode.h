// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WinWAudioEventTestGameMode.generated.h"

class FRunnableThread;
class MirroringVRContorolThread;

UCLASS(minimalapi)
class AWinWAudioEventTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWinWAudioEventTestGameMode();
	~AWinWAudioEventTestGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void StartMirroringVRControl();

	FRunnableThread* m_mirroringVRContorolThread;
	MirroringVRContorolThread* m_mirroringVRContorolRunnable;
};



