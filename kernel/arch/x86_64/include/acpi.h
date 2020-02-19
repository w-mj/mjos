#pragma once
#include <acpi_structure.h>
#include <base.h>


extern madt_t *acpi_madt;
extern mcfg_t *acpi_mcfg;
extern fadt_t *acpi_fadt;
extern hpet_t *acpi_hpet;

__INIT void acpi_tbl_init();

