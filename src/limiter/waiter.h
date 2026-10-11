// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
// high-res waitable timer sleep + short spin at the end
#pragma once

#include "../core/timing.h"

class PreciseWaiter {
public:
	PreciseWaiter();
	~PreciseWaiter();
	PreciseWaiter(const PreciseWaiter &) = delete;
	PreciseWaiter &operator=(const PreciseWaiter &) = delete;

	// returns how late we woke (ticks)
	timing::Ticks WaitUntil(timing::Ticks deadline);

	bool HighResolution() const { return highRes_; }
	double MarginMicroseconds() const { return timing::ToMicroseconds(margin_); }

private:
	void Sleep(timing::Ticks ticks);

	void *timer_ = nullptr;
	bool highRes_ = false;
	bool periodRaised_ = false;
	timing::Ticks margin_;    // spin part
	timing::Ticks worstLate_;
};
