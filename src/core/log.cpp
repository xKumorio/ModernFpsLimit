// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "log.h"

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

namespace logging {

static char g_path[MAX_PATH];

void Open(const char *path)
{
	strcpy_s(g_path, path);
	FILE *f = nullptr;
	if (!fopen_s(&f, g_path, "w") && f) fclose(f);
}

void Write(const char *fmt, ...)
{
	if (!g_path[0]) return;
	FILE *f = nullptr;
	if (fopen_s(&f, g_path, "a") || !f) return;
	SYSTEMTIME t;
	GetLocalTime(&t);
	fprintf(f, "[%02d:%02d:%02d] ", t.wHour, t.wMinute, t.wSecond);
	va_list args;
	va_start(args, fmt);
	vfprintf(f, fmt, args);
	va_end(args);
	fputc('\n', f);
	fclose(f);
}

} // namespace logging
