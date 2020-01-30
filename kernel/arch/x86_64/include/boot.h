#pragma once
#include <types.h>
#include <memory.h>
#include <attribute.h>

struct BootParameters {
	u32 memoryListLength;  /* 4B */
	struct DetectedMemoryEntry memoryList[10];  /* 240 */
};

extern struct BootParameters bootParameters;

#include <multiboot.h>
