#pragma once
#include <string>

void ctrl_volume_init();

void ctrl_volume_set_main(float vol);
float ctrl_volume_get_main();

void ctrl_volume_set_program(std::wstring exeName, float vol);

void ctrl_volume_iterate_sessions();
