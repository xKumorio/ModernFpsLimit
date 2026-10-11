// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include <windows.h>
#include <string.h>

#include "app.h"

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(instance);
		char folder[MAX_PATH];
		GetModuleFileNameA(instance, folder, MAX_PATH);
		if (char *slash = strrchr(folder, '\\')) *slash = 0;
		App::Get().Start(folder);
	}
	return TRUE;
}
