// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include <windows.h>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <vector>

#include "../src/limiter/limiter.h"

using timing::Ticks;

static int g_failed;

#define CHECK(cond, ...)                              \
	do {                                              \
		if (!(cond)) {                                \
			g_failed++;                               \
			printf("  FAILED %s: ", #cond);           \
			printf(__VA_ARGS__);                      \
			printf("\n");                             \
		}                                             \
	} while (0)

static void TestPacer()
{
	printf("pacer on a made-up clock\n");
	FramePacer p;
	CHECK(p.Begin(1000) == 0, "no limit set: never holds");

	p.SetPeriod(100);
	Ticks now = 5000;
	CHECK(p.Begin(now) == 0, "the first frame starts the grid");
	for (int i = 1; i <= 50; i++) {
		now += 30;
		Ticks hold = p.Begin(now);
		CHECK(hold == 5000 + i * 100, "frame %d held until %lld, expected %d", i, hold, 5000 + i * 100);
		now = hold;
	}
	now += 130;
	CHECK(p.Begin(now) == 0, "a late frame starts at once");
	now += 30;
	Ticks hold = p.Begin(now);
	CHECK(hold == 5000 + 52 * 100, "the frame after a late one is on the old grid: %lld", hold);
	now = hold;
	now += 500;
	CHECK(p.Begin(now) == 0, "the frame after a freeze starts at once");
	Ticks after = now;
	now += 30;
	hold = p.Begin(now);
	CHECK(hold == after + 100, "after a freeze the grid restarts: %lld, expected %lld", hold, after + 100);
	CHECK(p.Count().late == 1 && p.Count().resynced == 1, "late %u resynced %u", p.Count().late, p.Count().resynced);

	p.SetPeriod(100);
	now = 100000;
	unsigned heldBefore = p.Count().waited;
	for (int i = 0; i < 100; i++) {
		now += 140;
		CHECK(p.Begin(now) == 0, "a frame slower than the period is not held");
	}
	CHECK(p.Count().waited == heldBefore, "nothing was held");
}

static void Work(double ms)
{
	Ticks end = timing::Now() + Ticks(ms / 1000.0 * timing::PerSecond());
	while (timing::Now() < end) {}
}

static void TestRate(int fps, double workMs, double seconds)
{
	FrameLimiter limiter;
	limiter.SetFps(fps);
	std::vector<Ticks> starts;
	Ticks end = timing::Now() + Ticks(seconds * timing::PerSecond());
	while (timing::Now() < end) {
		limiter.FrameStart();
		starts.push_back(timing::Now());
		Work(workMs);
	}
	size_t skip = starts.size() / 10 + 2;
	double period = 1e6 / fps, sum = 0;
	std::vector<double> off;
	for (size_t i = skip + 1; i < starts.size(); i++) {
		double us = timing::ToMicroseconds(starts[i] - starts[i - 1]);
		sum += us;
		off.push_back(fabs(us - period));
	}
	std::sort(off.begin(), off.end());
	double avg = sum / off.size(), reached = 1e6 / avg, jitter = off[off.size() * 95 / 100], worst = off.back();
	const FrameLimiter::Stats &s = limiter.LastSecond();
	printf("  limit %4d, work %.1f ms: %8.2f FPS, 95%% of frames within %5.1f us of the period (worst %7.1f us), start error %5.1f us, spin margin %.0f us\n", fps, workMs,
	       reached, jitter, worst, s.errorMicroseconds, limiter.Waiter().MarginMicroseconds());
	CHECK(fabs(reached - fps) < fps * 0.005, "limit %d: reached %.2f FPS", fps, reached);
	CHECK(jitter < 150, "limit %d: 95%% of frames within %.1f us", fps, jitter);
}

int main()
{
	TestPacer();
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	FrameLimiter probe;
	printf("limiter on the real clock (%s)\n", probe.Waiter().HighResolution() ? "high-resolution timer" : "1 ms system timer");
	TestRate(60, 2.0, 2.0);
	TestRate(150, 2.3, 2.0);
	TestRate(150, 5.5, 2.0);
	TestRate(240, 1.5, 2.0);
	TestRate(500, 0.5, 2.0);
	printf("%s\n", g_failed ? "FAILED" : "PASSED");
	return g_failed ? 1 : 0;
}
