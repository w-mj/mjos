#pragma once
#include <types.h>

struct DetectedMemoryEntry {
	u64 address;
	u64 length;
	u32 type;
	u32 addition;
}; /* 24B */
