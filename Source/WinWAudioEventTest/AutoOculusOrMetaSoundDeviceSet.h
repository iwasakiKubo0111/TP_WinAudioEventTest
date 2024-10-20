#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <string>
#include <propvarutil.h>
#include "CoreMinimal.h" // UE_LOG���g�p���邽�߂ɕK�v

// IPolicyConfig���`
struct IPolicyConfig : IUnknown {
    virtual HRESULT SetDefaultEndPoint(LPCWSTR deviceId, ERole role) = 0;
};

// GUID���`
EXTERN_C const CLSID CLSID_PolicyConfig = { 0xA9C22E17, 0xA4D0, 0x4E85, { 0x83, 0x59, 0xAA, 0x03, 0xA4, 0xCC, 0xE9, 0x8D } };
EXTERN_C const IID IID_IPolicyConfig = { 0xD6666E3D, 0x5B89, 0x4D98, { 0x8E, 0x7C, 0xA6, 0xE4, 0x02, 0xE5, 0xE7, 0x16 } };
// UE5�p�̐ÓI�ȃI�[�f�B�I�f�o�C�X�Z���N�^
class AutoOculusOrMetaSoundDeviceSet
{
public:
    // Meta�܂���Oculus�̖��O���܂ލĐ��f�o�C�X��I������
    // ��������true�A���s����false��Ԃ�
    static bool SelectAudioDeviceWithMetaOrOculus() {
        CoInitialize(nullptr);

        IMMDeviceEnumerator* pEnumerator = nullptr;
        IMMDeviceCollection* pCollection = nullptr;
        IMMDevice* pDevice = nullptr;
        bool deviceFound = false;

        // �f�o�C�X�񋓎q�̎擾
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
        if (FAILED(hr)) {
            UE_LOG(LogTemp, Error, TEXT("Failed to create device enumerator!"));
            CoUninitialize();
            return false;
        }

        // �Đ��f�o�C�X�̗� (eRender = �Đ��f�o�C�X)
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        if (FAILED(hr)) {
            UE_LOG(LogTemp, Error, TEXT("Failed to enumerate audio endpoints!"));
            pEnumerator->Release();
            CoUninitialize();
            return false;
        }

        UINT deviceCount;
        pCollection->GetCount(&deviceCount);

        // �e�f�o�C�X���`�F�b�N
        for (UINT i = 0; i < deviceCount; ++i) {
            hr = pCollection->Item(i, &pDevice);
            if (SUCCEEDED(hr)) {
                IPropertyStore* pProps = nullptr;
                hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
                if (SUCCEEDED(hr)) {
                    PROPVARIANT varName;
                    PropVariantInit(&varName);

                    // �f�o�C�X���̎擾
                    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
                    if (SUCCEEDED(hr)) {
                        std::wstring deviceName(varName.pwszVal);

                        // �f�o�C�X���� "Meta" �܂��� "Oculus" ���܂܂�Ă��邩�m�F
                        if (deviceName.find(L"Meta") != std::wstring::npos ||
                            deviceName.find(L"Oculus") != std::wstring::npos) {

                            UE_LOG(LogTemp, Log, TEXT("Selecting device: %s"), *FString(deviceName.c_str()));

                            // �f�o�C�X������̃f�o�C�X�Ƃ��Đݒ�
                            if (SetDefaultAudioDevice(deviceName.c_str())) {
                                deviceFound = true;
                                break;  // �f�o�C�X�����������烋�[�v�𔲂���
                            }
                        }
                    }
                    PropVariantClear(&varName);
                    pProps->Release();
                }
                pDevice->Release();
            }
        }

        // �������
        pCollection->Release();
        pEnumerator->Release();
        CoUninitialize();

        // �f�o�C�X��������ݒ�ł�����true�A���s������false��Ԃ�
        return deviceFound;
    }

private:
    // ����̃I�[�f�B�I�f�o�C�X��ݒ肷��
    static bool SetDefaultAudioDevice(LPCWSTR deviceId) {
        CoInitialize(nullptr);

        IPolicyConfig* pPolicyConfig = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_PolicyConfig, nullptr, CLSCTX_ALL, IID_IPolicyConfig, (void**)&pPolicyConfig);

        if (SUCCEEDED(hr)) {
            UE_LOG(LogTemp, Error, TEXT("audio set"));
            hr = pPolicyConfig->SetDefaultEndPoint(deviceId, eConsole); // eConsole: �R���\�[���p�̃I�[�f�B�I�f�o�C�X�Ƃ��Đݒ�
            pPolicyConfig->Release();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("audio set failed, HRESULT: 0x%08X"), hr);
        }

        CoUninitialize();
        return SUCCEEDED(hr);
    }
};
