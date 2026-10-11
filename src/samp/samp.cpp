// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "samp.h"

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

#include "../core/log.h"

namespace samp {

namespace {

// R1/R3-1/R5-1/DL from SAMP-API, R2/R4 found by hand
struct Version {
	const char *name;
	DWORD entryPoint;
	DWORD inputRef;
	DWORD chatRef;
	DWORD addCommand;
	DWORD addMessage;
	DWORD netGameRef;
	DWORD netState;
	int connected;
};

const Version kVersions[] = {
	{ "0.3.7-R1", 0x31DF13, 0x21A0E8, 0x21A0E4, 0x65AD0, 0x645A0, 0x21A0F8, 0x3BD, 14 },
	{ "0.3.7-R2", 0x3195DD, 0x21A0F0, 0x21A0EC, 0x65BA0, 0x64670, 0x21A100, 0x3B9, 14 },
	{ "0.3.7-R3-1", 0xCC4D0, 0x26E8CC, 0x26E8C8, 0x69000, 0x679F0, 0x26E8DC, 0x3CD, 5 },
	{ "0.3.7-R4", 0xCBCB0, 0x26E9FC, 0x26E9F8, 0x69730, 0x68130, 0x26EA0C, 0x3CD, 5 },
	{ "0.3.7-R5-1", 0xCBC90, 0x26EB84, 0x26EB80, 0x69770, 0x68170, 0x26EB94, 0x3CD, 5 },
	{ "0.3.DL-R1", 0xFDB60, 0x2ACA14, 0x2ACA10, 0x691B0, 0x67BE0, 0x2ACA24, 0x3CD, 5 },
};

BYTE *g_base;
const Version *g_version;
bool g_unknown;

void *Input() { return *(void **)(g_base + g_version->inputRef); }
void *Chat() { return *(void **)(g_base + g_version->chatRef); }

bool Identify()
{
	HMODULE module = GetModuleHandleA("samp.dll");
	if (!module) return false;
	auto dos = (IMAGE_DOS_HEADER *)module;
	auto nt = (IMAGE_NT_HEADERS *)((BYTE *)module + dos->e_lfanew);
	DWORD entry = nt->OptionalHeader.AddressOfEntryPoint;
	for (const Version &v : kVersions)
		if (v.entryPoint == entry) {
			g_base = (BYTE *)module;
			g_version = &v;
			LOG("SA-MP %s", v.name);
			return true;
		}
	g_unknown = true;
	LOG("unknown samp.dll (ep %lX), no /setfps", entry);
	return false;
}

} // namespace

bool Ready()
{
	if (g_unknown) return false;
	if (!g_version) {
		static DWORD lastLook;
		DWORD now = GetTickCount();
		if (now - lastLook < 250) return false;
		lastLook = now;
		if (!Identify()) return false;
	}
	return Input() && Chat();
}

const char *VersionName() { return g_version ? g_version->name : nullptr; }

bool Connected()
{
	if (!g_version) return false;
	BYTE *net = *(BYTE **)(g_base + g_version->netGameRef);
	return net && *(int *)(net + g_version->netState) == g_version->connected;
}

bool AddCommand(const char *name, CommandHandler handler)
{
	if (!g_version || !Input()) return false;
	((void(__thiscall *)(void *, const char *, CommandHandler))(g_base + g_version->addCommand))(Input(), name, handler);
	return true;
}

void ChatMessage(unsigned long color, const char *utf8Format, ...)
{
	if (!g_version || !Chat()) return;
	char utf8[512];
	va_list args;
	va_start(args, utf8Format);
	_vsnprintf_s(utf8, _TRUNCATE, utf8Format, args);
	va_end(args);
	// utf8 -> cp1251
	wchar_t wide[512];
	char text[512];
	if (!MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide, 512)) return;
	if (!WideCharToMultiByte(1251, 0, wide, -1, text, sizeof(text), nullptr, nullptr)) return;
	text[143] = 0;
	((void(__thiscall *)(void *, unsigned long, const char *))(g_base + g_version->addMessage))(Chat(), color, text);
}

} // namespace samp
