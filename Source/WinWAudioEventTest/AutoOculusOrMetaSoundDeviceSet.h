#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <string>
#include <propvarutil.h>
#include "CoreMinimal.h" // UE_LOGを使用するために必要

// IPolicyConfigを定義
struct IPolicyConfig : IUnknown {
    virtual HRESULT SetDefaultEndPoint(LPCWSTR deviceId, ERole role) = 0;
};

// GUIDを定義
EXTERN_C const CLSID CLSID_PolicyConfig = { 0xA9C22E17, 0xA4D0, 0x4E85, { 0x83, 0x59, 0xAA, 0x03, 0xA4, 0xCC, 0xE9, 0x8D } };
EXTERN_C const IID IID_IPolicyConfig = { 0xD6666E3D, 0x5B89, 0x4D98, { 0x8E, 0x7C, 0xA6, 0xE4, 0x02, 0xE5, 0xE7, 0x16 } };
// UE5用の静的なオーディオデバイスセレクタ
class AutoOculusOrMetaSoundDeviceSet
{
public:
    // MetaまたはOculusの名前を含む再生デバイスを選択する
    // 成功時にtrue、失敗時にfalseを返す
    static bool SelectAudioDeviceWithMetaOrOculus() {
        CoInitialize(nullptr);

        IMMDeviceEnumerator* pEnumerator = nullptr;
        IMMDeviceCollection* pCollection = nullptr;
        IMMDevice* pDevice = nullptr;
        bool deviceFound = false;

        // デバイス列挙子の取得
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
        if (FAILED(hr)) {
            UE_LOG(LogTemp, Error, TEXT("Failed to create device enumerator!"));
            CoUninitialize();
            return false;
        }

        // 再生デバイスの列挙 (eRender = 再生デバイス)
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        if (FAILED(hr)) {
            UE_LOG(LogTemp, Error, TEXT("Failed to enumerate audio endpoints!"));
            pEnumerator->Release();
            CoUninitialize();
            return false;
        }

        UINT deviceCount;
        pCollection->GetCount(&deviceCount);

        // 各デバイスをチェック
        for (UINT i = 0; i < deviceCount; ++i) {
            hr = pCollection->Item(i, &pDevice);
            if (SUCCEEDED(hr)) {
                IPropertyStore* pProps = nullptr;
                hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
                if (SUCCEEDED(hr)) {
                    PROPVARIANT varName;
                    PropVariantInit(&varName);

                    // デバイス名の取得
                    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
                    if (SUCCEEDED(hr)) {
                        std::wstring deviceName(varName.pwszVal);

                        // デバイス名に "Meta" または "Oculus" が含まれているか確認
                        if (deviceName.find(L"Meta") != std::wstring::npos ||
                            deviceName.find(L"Oculus") != std::wstring::npos) {

                            UE_LOG(LogTemp, Log, TEXT("Selecting device: %s"), *FString(deviceName.c_str()));

                            // デバイスを既定のデバイスとして設定
                            if (SetDefaultAudioDevice(deviceName.c_str())) {
                                deviceFound = true;
                                break;  // デバイスが見つかったらループを抜ける
                            }
                        }
                    }
                    PropVariantClear(&varName);
                    pProps->Release();
                }
                pDevice->Release();
            }
        }

        // 解放処理
        pCollection->Release();
        pEnumerator->Release();
        CoUninitialize();

        // デバイスが見つかり設定できたらtrue、失敗したらfalseを返す
        return deviceFound;
    }

private:
    // 既定のオーディオデバイスを設定する
    static bool SetDefaultAudioDevice(LPCWSTR deviceId) {
        CoInitialize(nullptr);

        IPolicyConfig* pPolicyConfig = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_PolicyConfig, nullptr, CLSCTX_ALL, IID_IPolicyConfig, (void**)&pPolicyConfig);

        if (SUCCEEDED(hr)) {
            UE_LOG(LogTemp, Error, TEXT("audio set"));
            hr = pPolicyConfig->SetDefaultEndPoint(deviceId, eConsole); // eConsole: コンソール用のオーディオデバイスとして設定
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
