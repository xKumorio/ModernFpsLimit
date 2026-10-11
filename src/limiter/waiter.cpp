// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "waiter.h"

#include <emmintrin.h>
#include <mmsystem.h>

using timing::Ticks;

namespace {
const DWORD kHighResolutionTimer = 0x2; // CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
const long long kMinMarginUs = 150, kMaxMarginUs = 2500;
} // namespace

PreciseWaiter::PreciseWaiter()
{
	timer_ = CreateWaitableTimerExW(nullptr, nullptr, kHighResolutionTimer, TIMER_ALL_ACCESS);
	highRes_ = timer_ != nullptr;
	if (!highRes_) {
		timer_ = CreateWaitableTimerW(nullptr, TRUE, nullptr);
		periodRaised_ = timeBeginPeriod(1) == TIMERR_NOERROR;
	}
	worstLate_ = timing::FromMicroseconds(highRes_ ? 400 : 1500);
	margin_ = worstLate_ + timing::FromMicroseconds(100);
}

PreciseWaiter::~PreciseWaiter()
{
	if (timer_) CloseHandle((HANDLE)timer_);
	if (periodRaised_) timeEndPeriod(1);
}

void PreciseWaiter::Sleep(Ticks ticks)
{
	LARGE_INTEGER due;
	due.QuadPart = -(ticks * 10000000 / timing::PerSecond()); // relative, 100 ns units
	if (due.QuadPart < 0 && timer_ && SetWaitableTimer((HANDLE)timer_, &due, 0, nullptr, nullptr, FALSE)) {
		WaitForSingleObject((HANDLE)timer_, 1000);
		return;
	}
	::Sleep(1);
}

Ticks PreciseWaiter::WaitUntil(Ticks deadline)
{
	Ticks now = timing::Now();
	if (deadline - now <= 0) return now - deadline;
	// boost only for the sleep so we get the core back on wake
	HANDLE thread = GetCurrentThread();
	int priority = GetThreadPriority(thread);
	bool raised = priority != THREAD_PRIORITY_ERROR_RETURN && priority < THREAD_PRIORITY_TIME_CRITICAL && SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL);
	for (int i = 0; i < 2 && deadline - now > margin_ + timing::FromMicroseconds(100); i++) {
		Ticks wake = deadline - margin_;
		Sleep(wake - now);
		now = timing::Now();
		Ticks late = now - wake;
		worstLate_ -= worstLate_ / 256;
		if (late > worstLate_) worstLate_ = late;
		margin_ = worstLate_ + timing::FromMicroseconds(100);
		const Ticks lo = timing::FromMicroseconds(kMinMarginUs), hi = timing::FromMicroseconds(kMaxMarginUs);
		if (margin_ < lo) margin_ = lo;
		if (margin_ > hi) margin_ = hi;
	}
	if (raised) SetThreadPriority(thread, priority);
	while ((now = timing::Now()) < deadline) _mm_pause();
	return now - deadline;
}
