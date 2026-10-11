// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "app.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/log.h"
#include "game/gta.h"
#include "plugin.h"
#include "samp/samp.h"

namespace {

const unsigned long kColorText = 0xFFE8E4F5, kColorError = 0xFFFF6B86;
const char kTag[] = "{FF6EC7}[" PLUGIN_NAME "]{E8E4F5} ";

void FrameStartThunk() { App::Get().OnFrameStart(); }
void __cdecl SetFpsThunk(const char *params) { App::Get().OnSetFpsCommand(params); }

} // namespace

App &App::Get()
{
	static App app;
	return app;
}

void App::Start(const char *folder)
{
	char path[MAX_PATH];
	sprintf_s(path, "%s\\" PLUGIN_NAME ".log", folder);
	logging::Open(path);
	LOG(PLUGIN_NAME " v" PLUGIN_VERSION " - (c) 2026 " PLUGIN_AUTHOR ", " PLUGIN_AUTHOR_URL ". All rights reserved.");
	sprintf_s(path, "%s\\" PLUGIN_NAME ".ini", folder);
	config_.Load(path);
	limiter_.SetFps(config_.fps);
	config_.fps = limiter_.Fps();
	if (!gta::HookFrameStart(FrameStartThunk)) return;
	if (config_.abovePriority && GetPriorityClass(GetCurrentProcess()) == NORMAL_PRIORITY_CLASS && SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS))
		LOG("Process priority: above normal");
	LOG("Limit %d FPS, %s", limiter_.Fps(),
	    limiter_.Waiter().HighResolution() ? "high-resolution timer" : "no high-res timer, using timeBeginPeriod(1)");
}

void App::OnFrameStart()
{
	bool held = limiter_.FrameStart();
	// input that came during the wait
	if (held && config_.freshInput) gta::PumpMessages();
	if (limiter_.Fps() && config_.gameLimiterOff) gta::TurnBuiltInLimiterOff();
	if (!sampAttached_ && samp::Ready()) AttachToSamp();
	if (sampAttached_ && config_.greeting) Greet();
}

void App::AttachToSamp()
{
	sampAttached_ = true;
	if (!samp::AddCommand("setfps", SetFpsThunk)) return;
	LOG("/setfps registered");
}

void App::Greet()
{
	if (!samp::Connected()) {
		greeted_ = false;
		connectedAt_ = 0;
		return;
	}
	if (greeted_) return;
	DWORD now = GetTickCount();
	if (!connectedAt_) connectedAt_ = now ? now : 1;
	if (now - connectedAt_ < 3000) return;
	greeted_ = true;
	samp::ChatMessage(0xFFFFFFFF, "{FF6EC7}<" PLUGIN_NAME "> {FFFFFF}by {FF6EC7}" PLUGIN_AUTHOR " {FFFFFF}loaded! {FF6EC7}/setfps <число>");
}

void App::SetFps(int fps)
{
	limiter_.SetFps(fps);
	config_.fps = limiter_.Fps();
	config_.SaveFps();
	LOG("Limit set to %d FPS", limiter_.Fps());
}

void App::SayStatus()
{
	const FrameLimiter::Stats &s = limiter_.LastSecond();
	if (!limiter_.Fps()) {
		samp::ChatMessage(kColorText, "%sлимита нет, сейчас {FF6EC7}%.0f FPS{E8E4F5}. Поставить: /setfps <число от %d до %d>", kTag, s.fps,
		                  FrameLimiter::kMinFps, FrameLimiter::kMaxFps);
		return;
	}
	samp::ChatMessage(kColorText, "%sлимит {FF6EC7}%d FPS{E8E4F5}, сейчас %.1f. Кадр ждёт %.2f мс, точность %.0f мкс, не успело кадров: %.1f%%", kTag,
	                  limiter_.Fps(), s.fps, s.waitedMs, s.errorMicroseconds, s.latePercent);
}

void App::OnSetFpsCommand(const char *params)
{
	while (params && *params == ' ') params++;
	if (!params || !*params) {
		SayStatus();
		return;
	}
	char *end = nullptr;
	long fps = strtol(params, &end, 10);
	while (*end == ' ') end++;
	if (end == params || *end || (fps != 0 && (fps < FrameLimiter::kMinFps || fps > FrameLimiter::kMaxFps))) {
		samp::ChatMessage(kColorError, "%s{FF6B86}напиши /setfps <число от %d до %d>, /setfps 0 - без лимита", kTag, FrameLimiter::kMinFps,
		                  FrameLimiter::kMaxFps);
		return;
	}
	SetFps((int)fps);
	if (fps) samp::ChatMessage(kColorText, "%sлимит {FF6EC7}%d FPS{E8E4F5}, сохранён", kTag, limiter_.Fps());
	else samp::ChatMessage(kColorText, "%sлимит убран", kTag);
}
