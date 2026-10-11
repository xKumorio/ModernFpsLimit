// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
//
// 748D98  push edi
// 748D99  push 1Ah             ; rsIDLE
// 748D9B  call RsEventHandler  ; frame: CTimer::Update -> pads -> process -> render -> Present
// game limiter: 0xBA6794 (m_bFrameLimiterOn), SA-MP /fpslimit uses it too

#include "gta.h"

#include <windows.h>

#include "../core/log.h"
#include "../core/patch.h"

namespace gta {

namespace {

BYTE *const kIdleEventCall = (BYTE *)0x748D9B;
const BYTE kIdleEventArgs[] = { 0x57, 0x6A, 0x1A }; // push edi; push 1Ah
BYTE *const kFrameLimiterOn = (BYTE *)0xBA6794;

typedef int(__cdecl *EventHandler)(int event, void *param);
EventHandler g_next;
void (*g_onFrameStart)();

int __cdecl IdleEventHook(int event, void *param)
{
	g_onFrameStart();
	return g_next(event, param);
}

} // namespace

bool HookFrameStart(void (*onFrameStart)())
{
	BYTE *args = kIdleEventCall - sizeof(kIdleEventArgs);
	if (!patch::Readable(args, sizeof(kIdleEventArgs) + 5) || memcmp(args, kIdleEventArgs, sizeof(kIdleEventArgs)) || !patch::IsCall(kIdleEventCall)) {
		LOG("not gta_sa 1.0 US, limiter off");
		return false;
	}
	g_onFrameStart = onFrameStart;
	g_next = (EventHandler)patch::CallTarget(kIdleEventCall);
	if (!patch::RedirectCall(kIdleEventCall, (void *)IdleEventHook)) {
		LOG("patch failed, limiter off");
		return false;
	}
	LOG("hooked 748D9B (old %p)", (void *)g_next);
	return true;
}

void PumpMessages()
{
	MSG msg;
	while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE | PM_NOYIELD)) {
		if (msg.message == WM_QUIT) { // give it back to the main loop
			PostQuitMessage((int)msg.wParam);
			return;
		}
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

void TurnBuiltInLimiterOff()
{
	if (*kFrameLimiterOn) *kFrameLimiterOn = 0;
}

} // namespace gta
