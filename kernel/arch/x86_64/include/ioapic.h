#pragma once

#include <acpi.h>

typedef struct madt_ioapic       madr_ioapic_t;
typedef struct madt_int_override madt_int_override_t;

int  ioapic_irq_to_gsi(int irq);
int  ioapic_gsi_to_vec(int gsi);
void ioapic_gsi_mask  (int gsi);
void ioapic_gsi_unmask(int gsi);

__INIT void ioapic_dev_add(madt_ioapic_t * tbl);
__INIT void ioapic_int_override(madt_int_override_t * tbl);
__INIT void ioapic_all_init();

