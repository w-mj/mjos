#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "acpi_structure.h"
#include "base.h"


extern madt_t *acpi_madt; // 中断设备表
extern mcfg_t *acpi_mcfg; // PCI-E
extern fadt_t *acpi_fadt; // 高精度时钟
extern hpet_t *acpi_hpet; // 固定acpi描述符表

__INIT void acpi_tbl_init();


#ifdef __cplusplus
}
#endif
