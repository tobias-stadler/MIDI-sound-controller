// CtrlMidi.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include "pch.h"
#include <iostream>
#include "ctrl_volume.h"
#include "ctrl_midi.h"
#include <Windows.h>


int main()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ctrl_volume_init();
	ctrl_volume_iterate_sessions();
	ctrl_midi_init();

	std::cin.get();

}


