// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "pacer.h"

void FramePacer::SetPeriod(timing::Ticks period)
{
	period_ = period > 0 ? period : 0;
	next_ = 0;
}

timing::Ticks FramePacer::Begin(timing::Ticks now)
{
	if (!period_) return 0;
	if (!next_) {
		next_ = now;
		return 0;
	}
	next_ += period_;
	if (next_ > now) {
		if (next_ - now > period_) next_ = now + period_;
		counters_.waited++;
		return next_;
	}
	if (now - next_ >= period_) {
		next_ = now;
		counters_.resynced++;
	} else {
		counters_.late++;
	}
	return 0;
}
