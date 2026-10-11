// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
#pragma once

#include <windows.h>

namespace timing {

using Ticks = long long;

inline Ticks Now()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	return t.QuadPart;
}

inline Ticks PerSecond()
{
	static const Ticks f = [] {
		LARGE_INTEGER q;
		QueryPerformanceFrequency(&q);
		return q.QuadPart;
	}();
	return f;
}

inline Ticks FromMicroseconds(long long us) { return us * PerSecond() / 1000000; }
inline double ToMicroseconds(Ticks t) { return double(t) * 1e6 / double(PerSecond()); }

} // namespace timing
