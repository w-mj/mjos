#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "dev/ahci_stru.h"
#include "types.h"

extern HBA_PORT *sata;

void ahci_init(u8 bus, u8 dev, u8 fun);
bool sata_read(HBA_PORT *port, u32 startl, u32 starth, u32 count, u64 buf);
#ifdef __cplusplus
}
#endif
