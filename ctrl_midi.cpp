#include "pch.h"
#include "ctrl_midi.h"
#include "ctrl_volume.h"
#include <Windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <iostream>
#include <cmath>

static HMIDIIN hMidiDev;
static uint8_t faderVals[FADER_NUM];

#pragma pack(1)
struct MidiMsg {
	uint8_t status;
	uint8_t midi1;
	uint8_t midi2;
	uint8_t unused;
};

static void CALLBACK midi_callback(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD param1, DWORD param2) {
	MidiMsg* midiMsg = reinterpret_cast<MidiMsg*>(&param1);
	//std::cout << "MIDI_CALLBACK" << std::endl;
	switch (wMsg) {

	case MIM_DATA:
		if (midiMsg->status == 0xB0) {
			if (midiMsg->midi1 >= FADER_NUM) {
				return;
			}
			faderVals[midiMsg->midi1] = midiMsg->midi2;
			//std::cout << (int)(midiMsg->midi1) << std::endl;
			//std::cout << (int)(midiMsg->midi2) << std::endl;
			float vol = midiMsg->midi2 / 125.0f;
			vol = fmin(1.0f, vol);
			if (midiMsg->midi1 == 0) {
				ctrl_volume_set_main(vol);
			}
			else if (midiMsg->midi1 == 1) {
				ctrl_volume_set_program(L"Spotify.exe", vol);
			}
			else if (midiMsg->midi1 == 2) {
				ctrl_volume_set_program(L"chrome.exe", vol);
			}
			else if (midiMsg->midi1 == 3) {
				ctrl_volume_set_program(L"system", vol);
			}
		}
	
		break;
	default:
		break;
	}
}

void ctrl_midi_init() {

	if (hMidiDev != NULL) return;

    int numDevs;
	numDevs = midiInGetNumDevs();
	
	MIDIINCAPS midiCaps;
	
	MMRESULT res;
	std::cout << "Found midi devices: " << std::endl;

	for (int i = 0; i < numDevs; i++) {
		midiInGetDevCaps(i,&midiCaps, sizeof(MIDIINCAPS));
		std::cout << "PortNum " << i << std::endl;
		std::wcout << L"Device Name: " << midiCaps.szPname << std::endl;
		std::cout << "----------------------------" << std::endl;
	}

	res = midiInOpen(&hMidiDev, 0, (DWORD_PTR)midi_callback, 0, CALLBACK_FUNCTION);
	if (res != MMSYSERR_NOERROR) {
		std::cerr << "Failed MIDI init" << std::endl;
		return;
	}
	midiInStart(hMidiDev);
}

void ctrl_midi_close() {
	if (hMidiDev == NULL) return;

	midiInStop(hMidiDev);
	midiInClose(hMidiDev);
	hMidiDev = NULL;
}


