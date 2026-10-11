// ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
// Copying, redistribution or reuse of this file without the author's permission is prohibited.

#include "patch.h"

#include <stdint.h>
#include <string.h>

namespace patch {

bool Readable(const void *address, size_t size)
{
	MEMORY_BASIC_INFORMATION mbi;
	const BYTE *p = (const BYTE *)address, *end = p + size;
	while (p < end) {
		if (!VirtualQuery(p, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT) return false;
		if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
		p = (const BYTE *)mbi.BaseAddress + mbi.RegionSize;
	}
	return true;
}

bool Write(void *address, const void *data, size_t size)
{
	DWORD old = 0;
	if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old)) return false;
	memcpy(address, data, size);
	VirtualProtect(address, size, old, &old);
	FlushInstructionCache(GetCurrentProcess(), address, size);
	return true;
}

bool IsCall(const void *site)
{
	return Readable(site, 5) && *(const BYTE *)site == 0xE8;
}

void *CallTarget(const void *site)
{
	const BYTE *p = (const BYTE *)site;
	int32_t rel;
	memcpy(&rel, p + 1, 4);
	return (void *)(p + 5 + rel);
}

void *RedirectCall(void *site, void *hook)
{
	if (!IsCall(site)) return nullptr;
	void *old = CallTarget(site);
	int32_t rel = (int32_t)((BYTE *)hook - ((BYTE *)site + 5));
	return Write((BYTE *)site + 1, &rel, 4) ? old : nullptr;
}

} // namespace patch
