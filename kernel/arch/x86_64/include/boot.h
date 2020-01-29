#pragma once
#include <types.h>
#include <memory.h>
#include <attribute.h>

struct BootParameters {
	u32 memoryListLength;  /* 4B */
	struct DetectedMemoryEntry memoryList[10];  /* 240 */
};

extern struct BootParameters bootParameters;


struct GRUBBootParams {
	u32 flags;
	u32 mem_lower;
	u32 mem_upper;
	u32 boot_device;

};
