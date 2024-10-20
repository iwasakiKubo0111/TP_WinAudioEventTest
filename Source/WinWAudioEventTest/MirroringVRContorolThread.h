#include "CoreMinimal.h"

#include "HAL/Runnable.h"

#include "VolumeChangeHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "WinWAudioEventTestCharacter.h" // プレイヤークラスのヘッダー
#include "HeadMountedDisplayFunctionLibrary.h"


class MirroringVRContorolThread : public FRunnable
{
public:
    // コンストラクタ
    MirroringVRContorolThread(AWinWAudioEventTestCharacter* player) : StopTaskCounter(0) 
    { 
        m_playerCharacter = player; 
        m_cameraComponent = player->FindComponentByClass<UCameraComponent>();
    }

    // デストラクタ
    virtual ~MirroringVRContorolThread() {}

    // スレッド開始前に呼ばれる初期化処理
    virtual bool Init() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Init"));
        
        //音量検知クラスを設定
        m_handler = new VolumeChangeHandler(m_playerCharacter);
        m_handler->StartVolumeMonitoring();

        return true;
    }

    // スレッドのメイン処理
    virtual uint32 Run() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Run"));

        while (StopTaskCounter.GetValue() == 0) // StopTaskCounter が 0 の間ループ
        {
            // ここにスレッドで行いたい処理を記述
            //FPlatformProcess::Sleep(3.05f); // スリープで処理を間隔を開ける
            //FRotator NewRotation = FRotator(0, 90, 0); // ピッチ, ヨー, ロール
            //m_cameraComponent->SetWorldRotation(NewRotation);
            //UE_LOG(LogTemp, Warning, TEXT("!!!"));

            FRotator HMDRotation;
            FVector HMDPosition;

            // HMDの位置と回転を取得
            UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDPosition);
        }

        return 0;
    }

    // スレッド終了時の処理
    virtual void Stop() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Stop"));
        StopTaskCounter.Increment();
    }

    // スレッド終了後のクリーンアップ処理
    virtual void Exit() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Exit"));
        m_handler->StopVolumeMonitoring();
        m_handler->Release();
        //MEMO StopVolumeMonitoring() を呼んだ後にRelease()を呼び出すことで参照カウントが0になり(Releaseメソッド側で)インスタンスが削除される。
    }

private:
    FThreadSafeCounter StopTaskCounter; // スレッドの停止フラグ
    VolumeChangeHandler* m_handler;
    UCameraComponent* m_cameraComponent;
    AWinWAudioEventTestCharacter* m_playerCharacter;
};
