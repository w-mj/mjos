#include <driver/ahci.h>
#include <driver/pci.h>
#include <types.h>
#include <delog.h>
#include <base.h>

#define    SATA_SIG_ATA    0x00000101    // SATA drive
#define    SATA_SIG_ATAPI    0xEB140101    // SATAPI drive
#define    SATA_SIG_SEMB    0xC33C0101    // Enclosure management bridge
#define    SATA_SIG_PM    0x96690101    // Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

static int check_type(HBA_PORT *port) {
    u32 ssts = port->ssts;

    u8 ipm = (ssts >> 8) & 0x0F;
    u8 det = ssts & 0x0F;

    if (det != HBA_PORT_DET_PRESENT)    // Check drive status
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;

    switch (port->sig) {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        case SATA_SIG_ATA:
            return AHCI_DEV_SATA;
        default:
            return AHCI_DEV_NULL;
    }
}

void ahci_init(u8 bus, u8 dev, u8 fun) {
    u32 reg3 = pci_read(bus, dev, fun, 0x0C);
    u32 ht = (reg3 >> 16) & 0xff;
    if (ht != 0) {
        loge("pci header type %d is not support", ht);
    }
    u32 base = pci_read(bus, dev, fun, 0x24);
    _sL(base);
    base = base & (~0xfff);
    HBA_MEM *hba_mem = (HBA_MEM *) (u64) (base);
    logi("ahci implemented %d ports", (hba_mem->cap & 0x1f) + 1);
    for (int i = 0; i < 32; i++) {
        if (_chk_bit(hba_mem->pi, i)) {
            int dt = check_type(&hba_mem->ports[i]);
            switch (dt) {
                case AHCI_DEV_SATA:
                    logd("SATA drive found at port %d", i);
                    break;
                case AHCI_DEV_SATAPI:
                    logd("SATAPI drive found at port %d", i);
                    break;
                case AHCI_DEV_SEMB:
                    logd("SEMB drive found at port %d", i);
                    break;
                case AHCI_DEV_PM:
                    logd("PM drive found at port %d", i);
                    break;
                default:
                    logd("No drive found at port %d", i);
            }
        }
    }
}