// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
#pragma once

namespace gta {

bool HookFrameStart(void (*onFrameStart)());

void PumpMessages();

void TurnBuiltInLimiterOff();

} // namespace gta
