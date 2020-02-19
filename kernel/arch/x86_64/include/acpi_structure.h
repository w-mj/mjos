#pragma once

#include <types.h>
#include <base.h>

typedef struct acpi_tbl {
    u32    signature;
    u32    length;
    u8     revision;
    u8     checksum;
    char   oem_id[6];
    char   oem_table_id[8];
    u32    oem_revision;
    u32    asl_compiler_id;
    u32    asl_compiler_revision;
} __PACKED acpi_tbl_t;

typedef struct acpi_subtbl {
    u8     type;
    u8     length;
} __PACKED acpi_subtbl_t;

typedef struct acpi_rsdp {
    u64     signature;
    u8      checksum;
    char    oem_id[6];
    u8      revision;
    u32     rsdt_addr;
    u32     length;         // this and following field are v2 only
    u64     xsdt_addr;
    u8      checksum_ext;
    u8      reserved[3];
} __PACKED acpi_rsdp_t;

typedef struct acpi_rsdt {
    acpi_tbl_t  header;
    u32         entries[0];
} __PACKED acpi_rsdt_t;

typedef struct acpi_xsdt {
    acpi_tbl_t  header;
    u64         entries[0];
} __PACKED acpi_xsdt_t;

// Multiple APIC Description Table
// 用于枚举中断设备
typedef struct madt {
    acpi_tbl_t  header;
    u32         loapic_addr;
    u32         flags;
} __PACKED madt_t;

#define MADT_TYPE_LOCAL_APIC             0
#define MADT_TYPE_IO_APIC                1
#define MADT_TYPE_INTERRUPT_OVERRIDE     2
#define MADT_TYPE_NMI_SOURCE             3
#define MADT_TYPE_LOCAL_APIC_NMI         4
#define MADT_TYPE_LOCAL_APIC_OVERRIDE    5
#define MADT_TYPE_IO_SAPIC               6
#define MADT_TYPE_LOCAL_SAPIC            7
#define MADT_TYPE_INTERRUPT_SOURCE       8
#define MADT_TYPE_LOCAL_X2APIC           9
#define MADT_TYPE_LOCAL_X2APIC_NMI      10
#define MADT_TYPE_GENERIC_INTERRUPT     11
#define MADT_TYPE_GENERIC_DISTRIBUTOR   12
#define MADT_TYPE_GENERIC_MSI_FRAME     13
#define MADT_TYPE_GENERIC_REDISTRIBUTOR 14
#define MADT_TYPE_GENERIC_TRANSLATOR    15
#define MADT_TYPE_RESERVED              16

// type = 0, Local APIC
typedef struct madt_loapic {
    acpi_subtbl_t header;
    u8            processor_id;
    u8            id;
    u8            loapic_flags;
} __PACKED madt_loapic_t;

// type = 1, IO APIC
typedef struct madt_ioapic {
    acpi_subtbl_t header;
    u8            id;
    u8            reserved;
    u32           address;
    u32           global_irq_base;
} __PACKED madt_ioapic_t;

// type = 2, Interrupt Override
typedef struct madt_int_override {
    acpi_subtbl_t header;
    u8            bus;          // always 0, meaning ISA
    u8            source_irq;
    u32           global_irq;
#define POLARITY_MASK       3
#define POLARITY_CONFIRM    0   // confirm to the specifications of the bus
                                // e.g. E-ISA is active-low, level-triggered
#define POLARITY_HIGH       1   // active high
#define POLARITY_LOW        3   // active low
#define TRIGMODE_MASK      12
#define TRIGMODE_CONFIRM    0   // conforms to specifications of the bus
                                // e.g. ISA is edge-triggered
#define TRIGMODE_EDGE       4   // edge-triggered
#define TRIGMODE_LEVEL     12   // level-triggered
    u16           inti_flags;
} __PACKED madt_int_override_t;

// type = 4, Local APIC Non Maskable Interrupts
typedef struct madt_loapic_mni {
    acpi_subtbl_t header;
    u8            processor_id;
    u16           flags;
    u8            lint;
} __PACKED madt_loapic_mni_t;

// type = 5, Address Override
typedef struct madt_loapic_override {
    acpi_subtbl_t header;
    u16           reserved;
    u64           address;
} __PACKED madt_loapic_override_t;

// type = 8, Platform Interrupt Source
typedef struct madt_int_source {
    acpi_subtbl_t header;
    u16           inti_flags;
    u8            type;
    u8            id;
    u8            eid;
    u8            io_sapic_vector;
    u32           global_irq;
    u32           flags;
} __PACKED madt_int_source_t;

//------------------------------------------------------------------------------
// mcfg, for pci-express

typedef struct pci_conf {
    u64         address;
    u16         seg_group;
    u8          bus_num_start;
    u8          bus_num_end;
    u32         reserved;
} __PACKED pci_conf_t;

typedef struct mcfg {
    acpi_tbl_t  header;
    u32         reserved;
    pci_conf_t  entries[0];
} __PACKED mcfg_t;

// Hpet (High Precision Event Timer)
typedef struct address_structure
{
    u8 address_space_id;    // 0 - system memory, 1 - system I/O
    u8 bit_width;
    u8 bit_offset;
    u8 access_size;
    u64 address;
} __PACKED address_structure_t;

typedef struct hpet {
    acpi_tbl_t  header;
	u8 hardware_rev_id;
    u8 comparator_count:5,
       counter_size:1,
       reserved:1,
       legacy_replacement:1;
    u16 pci_vendor_id;
    address_structure_t address;
    u8 hpet_number;
    u16 minimum_tick;
    u8 page_protection;
	
} __PACKED hpet_t;

// fadt Fixed ACPI Description Table
typedef struct fadt {
    acpi_tbl_t  header;
u32 FirmwareCtrl;
    u32 Dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    u8  Reserved;

    u8  PreferredPowerManagementProfile;
    u16 SCI_Interrupt;
    u32 SMI_CommandPort;
    u8  AcpiEnable;
    u8  AcpiDisable;
    u8  S4BIOS_REQ;
    u8  PSTATE_Control;
    u32 PM1aEventBlock;
    u32 PM1bEventBlock;
    u32 PM1aControlBlock;
    u32 PM1bControlBlock;
    u32 PM2ControlBlock;
    u32 PMTimerBlock;
    u32 GPE0Block;
    u32 GPE1Block;
    u8  PM1EventLength;
    u8  PM1ControlLength;
    u8  PM2ControlLength;
    u8  PMTimerLength;
    u8  GPE0Length;
    u8  GPE1Length;
    u8  GPE1Base;
    u8  CStateControl;
    u16 WorstC2Latency;
    u16 WorstC3Latency;
    u16 FlushSize;
    u16 FlushStride;
    u8  DutyOffset;
    u8  DutyWidth;
    u8  DayAlarm;
    u8  MonthAlarm;
    u8  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    u16 BootArchitectureFlags;
 
    u8  Reserved2;
    u32 Flags;
 
    // 12 byte structure; see below for details
    address_structure_t ResetReg;
 
    u8  ResetValue;
    u8  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    u64                X_FirmwareControl;
    u64                X_Dsdt;
 
    address_structure_t X_PM1aEventBlock;
    address_structure_t X_PM1bEventBlock;
    address_structure_t X_PM1aControlBlock;
    address_structure_t X_PM1bControlBlock;
    address_structure_t X_PM2ControlBlock;
    address_structure_t X_PMTimerBlock;
    address_structure_t X_GPE0Block;
    address_structure_t X_GPE1Block;
} __PACKED fadt_t;

