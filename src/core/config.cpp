// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "config.h"

#include <windows.h>
#include <stdio.h>

#include "../plugin.h"

namespace {

const char kSection[] = PLUGIN_NAME;

// no BOM, GetPrivateProfile* chokes on it
const char kDefaultFile[] =
	"; " PLUGIN_NAME " " PLUGIN_VERSION " - (c) 2026 " PLUGIN_AUTHOR ", " PLUGIN_AUTHOR_URL "\r\n"
	"; Лимит FPS для GTA SA / SA-MP, который ждёт ДО чтения клавиш и мыши: ровные кадры и меньше инпутлаг.\r\n"
	"; В игре: /setfps 150 - поставить лимит, /setfps 0 - убрать, /setfps - показать текущий.\r\n"
	"\r\n"
	"[" PLUGIN_NAME "]\r\n"
	"; лимит кадров в секунду (20-1000), 0 = без лимита. Команда /setfps меняет и сохраняет его сюда\r\n"
	"fps=150\r\n"
	"; 1 = нажатия клавиш, пришедшие за время ожидания, попадают в этот же кадр (меньше задержка клавиатуры)\r\n"
	"fresh_input=1\r\n"
	"; 1 = держать выключенным встроенный в игру Frame Limiter, пока включён этот лимит (иначе их два)\r\n"
	"game_limiter_off=1\r\n"
	"; 1 = строка в чате SA-MP при подключении\r\n"
	"greeting=1\r\n"
	"; 1 = приоритет игры \"выше среднего\": браузер и другие программы не отбирают у неё ядро посреди кадра\r\n"
	"above_priority=1\r\n";

} // namespace

void Config::Load(const char *path)
{
	strcpy_s(path_, path);
	if (GetFileAttributesA(path_) == INVALID_FILE_ATTRIBUTES) {
		FILE *f = nullptr;
		if (!fopen_s(&f, path_, "wb") && f) {
			fwrite(kDefaultFile, 1, sizeof(kDefaultFile) - 1, f);
			fclose(f);
		}
	}
	fps = (int)GetPrivateProfileIntA(kSection, "fps", fps, path_);
	freshInput = GetPrivateProfileIntA(kSection, "fresh_input", freshInput, path_) != 0;
	gameLimiterOff = GetPrivateProfileIntA(kSection, "game_limiter_off", gameLimiterOff, path_) != 0;
	greeting = GetPrivateProfileIntA(kSection, "greeting", greeting, path_) != 0;
	abovePriority = GetPrivateProfileIntA(kSection, "above_priority", abovePriority, path_) != 0;
}

void Config::SaveFps() const
{
	if (!path_[0]) return;
	char value[16];
	sprintf_s(value, "%d", fps);
	WritePrivateProfileStringA(kSection, "fps", value, path_);
}
