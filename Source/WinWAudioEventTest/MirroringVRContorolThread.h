#include "CoreMinimal.h"

#include "HAL/Runnable.h"

#include "VolumeChangeHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "WinWAudioEventTestCharacter.h" // �v���C���[�N���X�̃w�b�_�[
#include "HeadMountedDisplayFunctionLibrary.h"


class MirroringVRContorolThread : public FRunnable
{
public:
    // �R���X�g���N�^
    MirroringVRContorolThread(AWinWAudioEventTestCharacter* player) : StopTaskCounter(0) 
    { 
        m_playerCharacter = player; 
        m_cameraComponent = player->FindComponentByClass<UCameraComponent>();
    }

    // �f�X�g���N�^
    virtual ~MirroringVRContorolThread() {}

    // �X���b�h�J�n�O�ɌĂ΂�鏉��������
    virtual bool Init() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Init"));
        
        //���ʌ��m�N���X��ݒ�
        m_handler = new VolumeChangeHandler(m_playerCharacter);
        m_handler->StartVolumeMonitoring();

        return true;
    }

    // �X���b�h�̃��C������
    virtual uint32 Run() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Run"));

        while (StopTaskCounter.GetValue() == 0) // StopTaskCounter �� 0 �̊ԃ��[�v
        {
            // �����ɃX���b�h�ōs�������������L�q
            //FPlatformProcess::Sleep(3.05f); // �X���[�v�ŏ������Ԋu���J����
            //FRotator NewRotation = FRotator(0, 90, 0); // �s�b�`, ���[, ���[��
            //m_cameraComponent->SetWorldRotation(NewRotation);
            //UE_LOG(LogTemp, Warning, TEXT("!!!"));

            FRotator HMDRotation;
            FVector HMDPosition;

            // HMD�̈ʒu�Ɖ�]���擾
            UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDPosition);
        }

        return 0;
    }

    // �X���b�h�I�����̏���
    virtual void Stop() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Stop"));
        StopTaskCounter.Increment();
    }

    // �X���b�h�I����̃N���[���A�b�v����
    virtual void Exit() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MirroringVRContorolThread Exit"));
        m_handler->StopVolumeMonitoring();
        m_handler->Release();
        //MEMO StopVolumeMonitoring() ���Ă񂾌��Release()���Ăяo�����ƂŎQ�ƃJ�E���g��0�ɂȂ�(Release���\�b�h����)�C���X�^���X���폜�����B
    }

private:
    FThreadSafeCounter StopTaskCounter; // �X���b�h�̒�~�t���O
    VolumeChangeHandler* m_handler;
    UCameraComponent* m_cameraComponent;
    AWinWAudioEventTestCharacter* m_playerCharacter;
};
