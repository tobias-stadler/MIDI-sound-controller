
#include "pch.h"
#include "ctrl_volume.h"
#include <Windows.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <iostream>


static IMMDeviceEnumerator *pDevEnum;
static IMMDevice *pMainDev;
static IAudioSessionManager2 *pMgr;
static IAudioSessionEnumerator *pSessEnum;
static IAudioEndpointVolume *pMasterVol;

static ISimpleAudioVolume *volLookup;

void ctrl_volume_init() {
	CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pDevEnum));

	pDevEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pMainDev);
	
	pMainDev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)(&pMgr));
	pMainDev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)(&pMasterVol));

	pMgr->GetSessionEnumerator(&pSessEnum);

}

void ctrl_volume_set_main(float vol) {
	pMasterVol->SetMasterVolumeLevelScalar(vol, NULL);
}

float ctrl_volume_get_main() {
	float level;
	pMasterVol->GetMasterVolumeLevelScalar(&level);
	return level;
}


void ctrl_volume_iterate_sessions() {
	int count;
	pSessEnum->GetCount(&count);

	for (int i = 0; i < count; i++) {
		IAudioSessionControl *pSessCtrl;
		IAudioSessionControl2 *pSessCtrl2;
		ISimpleAudioVolume *pAudioVol;
		pSessEnum->GetSession(i, &pSessCtrl);
		pSessCtrl->QueryInterface(&pSessCtrl2);
		pSessCtrl->QueryInterface(&pAudioVol);

		float level;
		pAudioVol->GetMasterVolume(&level);

		LPWSTR str;
		pSessCtrl2->GetSessionIdentifier(&str);

		std::wstring sstr(str);
		std::size_t startPos = sstr.find_last_of(L"\\");
		
		if (pSessCtrl2->IsSystemSoundsSession() == S_OK) {
			std::cout << "System Sound" << std::endl;
			std::cout << "Level: " << level << std::endl;
			std::cout << "----------------" << std::endl;
		}
		else if (startPos != std::string::npos) {
			std::wstring exeStr = sstr.substr(startPos+1, sstr.length() - startPos - 41);
			std::wcout << exeStr << std::endl;
			std::cout << "Level: " << level << std::endl;
			std::cout << "----------------" << std::endl;
		}
		CoTaskMemFree(str);
	}
}

void ctrl_volume_set_program(std::wstring exeName, float vol) {
	int count;
	pSessEnum->GetCount(&count);

	for (int i = 0; i < count; i++) {
		IAudioSessionControl* pSessCtrl;
		IAudioSessionControl2* pSessCtrl2;
		ISimpleAudioVolume* pAudioVol;
		pSessEnum->GetSession(i, &pSessCtrl);
		pSessCtrl->QueryInterface(&pSessCtrl2);
		pSessCtrl->QueryInterface(&pAudioVol);

		if (exeName.compare(L"system")== 0) {
			if (pSessCtrl2->IsSystemSoundsSession() == S_OK) {
				pAudioVol->SetMasterVolume(vol, NULL);
				break;
			}
			else {
				continue;
			}
		}

		LPWSTR str;
		pSessCtrl2->GetSessionIdentifier(&str);

		std::wstring sstr(str);
		std::size_t startPos = sstr.find_last_of(L"\\");

		if (startPos != std::string::npos) {
			std::wstring exeStr = sstr.substr(startPos + 1, sstr.length() - startPos - 41);
			if (exeStr.compare(exeName) == 0) {
				pAudioVol->SetMasterVolume(vol, NULL);
			}
		}
		CoTaskMemFree(str);
	}
}