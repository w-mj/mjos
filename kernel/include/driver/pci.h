#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

void pci_probe_all();
u32  pci_read (u8 bus, u8 dev, u8 func, u8 reg);
void pci_write(u8 bus, u8 dev, u8 func, u8 reg, u32 data);
#ifdef __cplusplus
}
#endif
