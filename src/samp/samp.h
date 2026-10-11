// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.
// samp.dll 0.3.7 R1/R2/R3-1/R4/R5-1, 0.3.DL-R1
#pragma once

namespace samp {

typedef void(__cdecl *CommandHandler)(const char *params);

bool Ready();

const char *VersionName();
bool Connected();

bool AddCommand(const char *name, CommandHandler handler);
void ChatMessage(unsigned long color, const char *utf8Format, ...);

} // namespace samp
