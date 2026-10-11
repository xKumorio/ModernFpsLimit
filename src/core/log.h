// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
#pragma once

namespace logging {

void Open(const char *path);
void Write(const char *fmt, ...);

} // namespace logging

#define LOG(...) logging::Write(__VA_ARGS__)
