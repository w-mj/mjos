#include <driver/ahci.h>
#include <driver/pci.h>
#include <types.h>
#include <delog.h>
#include <base.h>

#define SATA_SIG_ATA   0x00000101    // SATA drive
#define SATA_SIG_ATAPI 0xEB140101    // SATAPI drive
#define SATA_SIG_SEMB  0xC33C0101    // Enclosure management bridge
#define SATA_SIG_PM    0x96690101    // Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

HBA_PORT *sata = null;

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
    logi("ahci %x implemented %d ports", hba_mem->vs, (hba_mem->cap & 0x1f) + 1);
    for (int i = 0; i < 32; i++) {
        if (_chk_bit(hba_mem->pi, i)) {
            int dt = check_type(&hba_mem->ports[i]);
            switch (dt) {
                case AHCI_DEV_SATA:
                    logd("SATA drive found at port %d", i);
                    HBA_PORT *port = &hba_mem->ports[i];
                    _sx(port->cmd);
                    u32 sctl = port->sctl;
//                    sctl = (sctl & (~0xf)) | 0x1;
//                    port->sctl = sctl;
//                    for (int i = 0; i < 65536 * 10; i++)
//                        ;
                    u32 ssts = port->ssts;
                    logd("SATA status: %x, %x, %x", 0xf&(ssts>>8), 0xf&(ssts>>4), 0xf&ssts);
                    logd("SATA speed limit %x", 0xf & (sctl >> 4));
                    sata = port;
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

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES (1 << 30)
#include <string.h>
#include <memory/page.h>

// Find a free command list slot
int find_cmdslot(HBA_PORT *port)
{
    int cmdslots = 32;
    // If not set in SACT and CI, the slot is free
    u32 slots = (port->sact | port->ci);
    for (int i=0; i<cmdslots; i++)
    {
        if ((slots&0) == 0)
            return i;
        slots >>= 1;
    }
    logd("Cannot find free command list entry\n");
    return -2;
}
#define print_port(p) { \
    _sx(p->clb); \
    _sx(p->clbu); \
    _sx(p->fb); \
    _sx(p->fbu); \
    _sx(p->is); \
    _sx(p->ie); \
    _sx(p->cmd); \
    _sx(p->rsv0); \
    _sx(p->tfd); \
    _sx(p->sig); \
    _sx(p->ssts); \
    _sx(p->sctl); \
    _sx(p->serr); \
    _sx(p->sact); \
    _sx(p->ci); \
    _sx(p->sntf); \
    _sx(p->fbs); \
    _sx(p->vendor[0]); \
    _sx(p->vendor[1]); \
    _sx(p->vendor[2]); \
    _sx(p->vendor[3]); \
}
#define print_fis(fb, fbu) do { \
    HBA_FIS* fis = (HBA_FIS *)(((u64)fbu << 32) + fb); \
    _ss("=====FIS===="); \
    if (fis->dsfis.fis_type != 0) {\
    _ss("=====DMA FIS===="); \
    _sx(fis->dsfis.fis_type); \
    _sx(fis->dsfis.pmport); \
    _sx(fis->dsfis.d); \
    _sx(fis->dsfis.i); \
    _sx(fis->dsfis.a); \
    _sx(fis->dsfis.DMAbufferID); \
    _sx(fis->dsfis.DMAbufOffset); \
    _sx(fis->dsfis.TransferCount);} \
    if (fis->psfis.fis_type != 0) {\
    _ss("=====PIO FIS===="); \
    _sx(fis->psfis.fis_type); \
    _sx(fis->psfis.d); \
    _sx(fis->psfis.i); \
    _sx(fis->psfis.status); \
    _sx(fis->psfis.error); \
    _sx(fis->psfis.lba0); \
    _sx(fis->psfis.lba1); \
    _sx(fis->psfis.lba2); \
    _sx(fis->psfis.device); \
    _sx(fis->psfis.lba3); \
    _sx(fis->psfis.lba4); \
    _sx(fis->psfis.lba5); \
    _sx(fis->psfis.countl); \
    _sx(fis->psfis.counth); \
    _sx(fis->psfis.e_status); \
    _sx(fis->psfis.tc); }\
    if (fis->rfis.fis_type != 0) {\
    _ss("======REG D2H FIS====="); \
    _sx(fis->rfis.fis_type); \
    _sx(fis->rfis.pmport); \
    _sx(fis->rfis.i); \
    _sx(fis->rfis.n); \
    _sx(fis->rfis.status); \
    _sx(fis->rfis.error); \
    _sx(fis->rfis.lba0); \
    _sx(fis->rfis.lba1); \
    _sx(fis->rfis.lba2); \
    _sx(fis->rfis.device); \
    _sx(fis->rfis.lba3); \
    _sx(fis->rfis.lba4); \
    _sx(fis->rfis.lba5); \
    _sx(fis->rfis.countl); \
    _sx(fis->rfis.counth); }\
    if (fis->sdbfis != 0) {\
    _ss("====SDB FIS======"); \
    _sL(fis->sdbfis);} \
    _ss("=======FIS END=========="); \
} while(0)
bool sata_read(HBA_PORT *port, u32 startl, u32 starth, u32 count, u8 *buf)
{
    assert(port != NULL);
    print_port(port);
    print_fis(port->fb, port->fbu);
    port->ie = (u32) -1;	// 打开中断
    // port->is = (u32) -1;	// Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot < 0)
        return false;

    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(u64)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(u32);	// Command FIS size
    cmdheader->w = 0;		// Read from device
    cmdheader->prdtl = (u16)((count-1)>>4) + 1;	// PRDT entries count

    if (cmdheader->ctba == 0) {
        pfn_t ctba = kernel_page_alloc(PG_KERNEL);
        cmdheader->ctba = ctba << PAGEOFFSET;
        cmdheader->ctbau = 0;
    }

    volatile HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)phys_to_virt(cmdheader->ctba);

    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
                      (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

    // 8K bytes (16 sectors) per PRDT
    int i;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (u32) buf;
        cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
        cmdtbl->prdt_entry[i].i = 1;
        buf += 8*1024;	// 4K words
        count -= 16;	// 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (u32) buf;
    cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;

    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = 0x25; // ATA_CMD_READ_DMA_EX;

    cmdfis->lba0 = (u8)startl;
    cmdfis->lba1 = (u8)(startl>>8);
    cmdfis->lba2 = (u8)(startl>>16);
    cmdfis->device = 1<<6;	// LBA mode

    cmdfis->lba3 = (u8)(startl>>24);
    cmdfis->lba4 = (u8)starth;
    cmdfis->lba5 = (u8)(starth>>8);

    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;

    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        logd("Port is hung\n");
        return false;
    }
    port->ie = 1;
    _sx(port->is);
    port->ci |= 1<<slot;	// Issue command

    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0)
            break;
        else {
            int a;
            a = 100;
        }
        if (port->is & HBA_PxIS_TFES)	// Task file error
        {
            logd("Read disk error\n");
            return false;
        }
    }

    // Check again
    if (port->is & HBA_PxIS_TFES)
    {
        logd("Read disk error\n");
        return false;
    }
    if (port->serr != 0) {
        loge("ERR");
    }
    print_fis(port->fb, port->fbu);
    print_port(port);

    return true;
}

