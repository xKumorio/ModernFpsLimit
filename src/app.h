// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
#pragma once

#include "core/config.h"
#include "limiter/limiter.h"

class App {
public:
	static App &Get();

	void Start(const char *folder);
	void OnFrameStart();
	void OnSetFpsCommand(const char *params);

private:
	void SetFps(int fps);
	void AttachToSamp();
	void Greet();
	void SayStatus();

	Config config_;
	FrameLimiter limiter_;
	bool sampAttached_ = false;
	bool greeted_ = false;
	unsigned long connectedAt_ = 0;
};
