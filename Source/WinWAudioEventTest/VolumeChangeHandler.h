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

    // ���ʂ��ς�����Ƃ��ɌĂяo�����R�[���o�b�N
    STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify"));
        if (pNotify->bMuted) 
        {
            // �~���[�g���ꂽ�ꍇ�̏���
            UE_LOG(LogTemp, Warning, TEXT("Muted"));
        }
        else 
        {
            if (UGameplayStatics::IsGamePaused(m_playerCharacter->GetWorld()))
            {
                //�|�[�Y�� = �P�����s�����i�K ���̎��͎��_���Z�b�g�����{����
                UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify viewRotation reset"));
                //m_cameraComponent->SetWorldRotation(m_initRotation);
                m_cameraComponent->SetRelativeRotation(m_initRotation);
            }
            else
            {
                //�{���ύX
                float newVolume = pNotify->fMasterVolume;
                if (m_currentVolume != newVolume)
                {
                    if (m_currentVolume < newVolume)
                    {
                        //8�{
                        UOpenXRBlueprintFunctionLibrary::SetEnvironmentBlendMode(1);//FIXME
                        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify 8�~"));
                        m_cameraComponent->SetRelativeRotation(m_initRotation);
                    }
                    else
                    {
                        //6�{
                        UOpenXRBlueprintFunctionLibrary::SetEnvironmentBlendMode(1);//FIXME
                        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::OnNotify 6�~"));
                    }
                    m_currentVolume = newVolume;
                }

            }
            float newVolume = pNotify->fMasterVolume;  // �V��������

            //FRotator NewRotation = FRotator(0, 45, 0); // �s�b�`, ���[, ���[��
            //m_cameraComponent->SetWorldRotation(NewRotation);
            //UE_LOG(LogTemp, Warning, TEXT("New Volume: %f"), newVolume);
        }
        return S_OK;
    }

    // IUnknown�C���^�[�t�F�C�X�̃��\�b�h
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

    // ���ʕύX�̊Ď����J�n����֐�
    void StartVolumeMonitoring() {
        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::StartVolumeMonitoring"));
        CoInitialize(NULL);
        //IMMDeviceEnumerator* pEnumerator = NULL;
        //IMMDevice* pDevice = NULL;
        //IAudioEndpointVolume* pVolume = NULL;

        // �f�o�C�X�񋓎q���쐬
        CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

        // �f�t�H���g�̃I�[�f�B�I�f�o�C�X���擾
        pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

        // �G���h�|�C���g�{�����[���C���^�[�t�F�[�X���擾
        pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);

        // �R�[���o�b�N��o�^
        VolumeChangeHandler* pHandler = new VolumeChangeHandler(m_playerCharacter);
        pVolume->RegisterControlChangeNotify(pHandler);

        // �N���[���A�b�v�͌�قǓK�؂ȏꏊ�ōs��
    }

    // �N���[���A�b�v�p�̊֐�
    void StopVolumeMonitoring() {
        UE_LOG(LogTemp, Warning, TEXT("Called VolumeChangeHandler::StopVolumeMonitoring"));
        if (pVolume) {
            // �R�[���o�b�N�̓o�^����
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
        // COM�̃N���[���A�b�v
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

