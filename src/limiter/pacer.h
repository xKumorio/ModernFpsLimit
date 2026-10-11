// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
// fixed grid: next frame = previous grid point + period, resync if a whole period late
#pragma once

#include "../core/timing.h"

class FramePacer {
public:
	struct Counters {
		unsigned waited = 0;
		unsigned late = 0;
		unsigned resynced = 0;
	};

	void SetPeriod(timing::Ticks period);
	timing::Ticks Period() const { return period_; }

	// returns deadline or 0 = go now
	timing::Ticks Begin(timing::Ticks now);

	const Counters &Count() const { return counters_; }
	void ResetCounters() { counters_ = {}; }

private:
	timing::Ticks period_ = 0;
	timing::Ticks next_ = 0;
	Counters counters_;
};
