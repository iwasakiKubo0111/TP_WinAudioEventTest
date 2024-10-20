#pragma once

#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include "Camera/CameraComponent.h"
#include "WinWAudioEventTestCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "OpenXRBlueprintFunctionLibrary.h"

//class AWinWAudioEventTestCharacter;

class VolumeChangeHandler : public IAudioEndpointVolumeCallback {
public:

    VolumeChangeHandler(AWinWAudioEventTestCharacter* player) 
    { 
        m_playerCharacter   = player;
        m_cameraComponent   = player->FindComponentByClass<UCameraComponent>();
        m_initRotation      = m_cameraComponent->GetComponentRotation();
    }

    // 音量が変わったときに呼び出されるコールバック
    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify"));
        if (pNotify->bMuted) 
        {
            // ミュートされた場合の処理
            UE_LOG(LogTemp, Warning, TEXT("Muted"));
        }
        else 
        {
            if (UGameplayStatics::IsGamePaused(m_playerCharacter->GetWorld()))
            {
                //ポーズ中 = 訓練実行準備段階 この時は視点リセットを実施する
                UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify viewRotation reset"));
                //m_cameraComponent->SetWorldRotation(m_initRotation);
                m_cameraComponent->SetRelativeRotation(m_initRotation);
            }
            else
            {
                //倍率変更
                float newVolume = pNotify->fMasterVolume;
                if (m_currentVolume != newVolume)
                {
                    if (m_currentVolume < newVolume)
                    {
                        //8倍
                        UOpenXRBlueprintFunctionLibrary::SetEnvironmentBlendMode(1);//FIXME
                        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify 8×"));
                        m_cameraComponent->SetRelativeRotation(m_initRotation);
                    }
                    else
                    {
                        //6倍
                        UOpenXRBlueprintFunctionLibrary::SetEnvironmentBlendMode(1);//FIXME
                        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify 6×"));
                    }
                    m_currentVolume = newVolume;
                }

            }
            float newVolume = pNotify->fMasterVolume;  // 新しい音量

            //FRotator NewRotation = FRotator(0, 45, 0); // ピッチ, ヨー, ロール
            //m_cameraComponent->SetWorldRotation(NewRotation);
            //UE_LOG(LogTemp, Warning, TEXT("New Volume: %f"), newVolume);
        }
        return S_OK;
    }

    // IUnknownインターフェイスのメソッド
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppvObject = static_cast<IAudioEndpointVolumeCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&refCount); }
    STDMETHODIMP_(ULONG) Release() {
        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::Release"));
        ULONG ulRef = InterlockedDecrement(&refCount);
        if (0 == ulRef) {
            delete this;
        }
        return ulRef;
    }

    // 音量変更の監視を開始する関数
    void StartVolumeMonitoring() {
        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::StartVolumeMonitoring"));
        CoInitialize(NULL);
        //IMMDeviceEnumerator* pEnumerator = NULL;
        //IMMDevice* pDevice = NULL;
        //IAudioEndpointVolume* pVolume = NULL;

        // デバイス列挙子を作成
        CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

        // デフォルトのオーディオデバイスを取得
        pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

        // エンドポイントボリュームインターフェースを取得
        pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);

        // コールバックを登録
        VolumeChangeHandler* pHandler = new VolumeChangeHandler(m_playerCharacter);
        pVolume->RegisterControlChangeNotify(pHandler);

        // クリーンアップは後ほど適切な場所で行う
    }

    // クリーンアップ用の関数
    void StopVolumeMonitoring() {
        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::StopVolumeMonitoring"));
        if (pVolume) {
            // コールバックの登録解除
            pVolume->UnregisterControlChangeNotify(this);
            pVolume->Release();
            pVolume = nullptr;
        }
        if (pDevice) {
            pDevice->Release();
            pDevice = nullptr;
        }
        if (pEnumerator) {
            pEnumerator->Release();
            pEnumerator = nullptr;
        }
        // COMのクリーンアップ
        CoUninitialize();
    }

private:
    LONG refCount = 1;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioEndpointVolume* pVolume = nullptr;
    UCameraComponent* m_cameraComponent;
    AWinWAudioEventTestCharacter* m_playerCharacter;
    FRotator m_initRotation;
    float m_currentVolume;
};

