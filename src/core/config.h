// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
#pragma once

struct Config {
	int fps = 150; // 0 = off
	bool freshInput = true;
	bool gameLimiterOff = true;
	bool greeting = true;
	bool abovePriority = true;

	void Load(const char *path);
	void SaveFps() const;

private:
	char path_[260] = "";
};
