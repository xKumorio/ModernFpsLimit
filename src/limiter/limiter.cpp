// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "limiter.h"

using timing::Ticks;

void FrameLimiter::SetFps(int fps)
{
	if (fps <= 0) fps = 0;
	else if (fps < kMinFps) fps = kMinFps;
	else if (fps > kMaxFps) fps = kMaxFps;
	fps_ = fps;
	pacer_.SetPeriod(fps ? timing::PerSecond() / fps : 0);
}

bool FrameLimiter::FrameStart()
{
	Ticks now = timing::Now();
	Ticks deadline = pacer_.Begin(now);
	Ticks error = 0, waited = 0;
	if (deadline) {
		error = waiter_.WaitUntil(deadline);
		waited = deadline + error - now;
		now = deadline + error;
	}
	Account(now, waited, error, deadline != 0);
	return deadline != 0;
}

void FrameLimiter::Account(Ticks now, Ticks waited, Ticks error, bool held)
{
	if (!windowStart_) windowStart_ = now;
	frames_++;
	held_ += held;
	sumWaited_ += waited;
	sumError_ += error;
	Ticks span = now - windowStart_;
	if (span < timing::PerSecond()) return;
	stats_.fps = double(frames_ - 1) * double(timing::PerSecond()) / double(span);
	stats_.latePercent = fps_ ? 100.0 * (frames_ - held_) / frames_ : 0.0;
	stats_.errorMicroseconds = held_ ? timing::ToMicroseconds(sumError_) / held_ : 0.0;
	stats_.waitedMs = timing::ToMicroseconds(sumWaited_) / frames_ / 1000.0;
	windowStart_ = now;
	frames_ = 1;
	held_ = 0;
	sumWaited_ = sumError_ = 0;
}
