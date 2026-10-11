// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
#pragma once

#include "pacer.h"
#include "waiter.h"

class FrameLimiter {
public:
	static const int kMinFps = 20, kMaxFps = 1000;

	struct Stats {
		double fps = 0;
		double latePercent = 0;
		double errorMicroseconds = 0;
		double waitedMs = 0;
	};

	void SetFps(int fps); // 0 = off
	int Fps() const { return fps_; }

	// true = waited
	bool FrameStart();

	const Stats &LastSecond() const { return stats_; }
	const PreciseWaiter &Waiter() const { return waiter_; }

private:
	void Account(timing::Ticks now, timing::Ticks waited, timing::Ticks error, bool held);

	FramePacer pacer_;
	PreciseWaiter waiter_;
	int fps_ = 0;

	Stats stats_;
	timing::Ticks windowStart_ = 0, sumWaited_ = 0, sumError_ = 0;
	unsigned frames_ = 0, held_ = 0;
};
