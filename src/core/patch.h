// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
#pragma once

#include <windows.h>

namespace patch {

bool Readable(const void *address, size_t size);
bool Write(void *address, const void *data, size_t size);

bool IsCall(const void *site);
void *CallTarget(const void *site);

// E8 call -> hook, returns old target (keeps other plugins in the chain)
void *RedirectCall(void *site, void *hook);

} // namespace patch
