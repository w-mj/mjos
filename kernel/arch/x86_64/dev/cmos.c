#include <asm.h>
#include <time.h>
#include <string.h>

#define CMOS_CTR_REG  0x70
#define CMOS_DATA_REG 0x71
#define NMI_DISABLE_BIT 0x80


#define CMOS_RTC_SECONDS         0x00
#define CMOS_RTC_SECONDS_ALARM   0x01
#define CMOS_RTC_MINUTES         0x02
#define CMOS_RTC_MINUTES_ALARM   0x03
#define CMOS_RTC_HOURS           0x04
#define CMOS_RTC_HOURS_ALARM     0x05
#define CMOS_RTC_DAY_WEEK        0x06
#define CMOS_RTC_DAY_MONTH       0x07
#define CMOS_RTC_MONTH           0x08
#define CMOS_RTC_YEAR            0x09
#define CMOS_STATUS_A            0x0a
#define CMOS_STATUS_B            0x0b
#define CMOS_STATUS_C            0x0c
#define CMOS_STATUS_D            0x0d
#define CMOS_RESET_CODE          0x0f
#define CMOS_FLOPPY_DRIVE_TYPE   0x10
#define CMOS_DISK_DATA           0x12
#define CMOS_EQUIPMENT_INFO      0x14
#define CMOS_DISK_DRIVE1_TYPE    0x19
#define CMOS_DISK_DRIVE2_TYPE    0x1a
#define CMOS_DISK_DRIVE1_CYL     0x1b
#define CMOS_DISK_DRIVE2_CYL     0x24
#define CMOS_MEM_EXTMEM_LOW      0x30
#define CMOS_MEM_EXTMEM_HIGH     0x31
#define CMOS_CENTURY             0x32
#define CMOS_MEM_EXTMEM2_LOW     0x34
#define CMOS_MEM_EXTMEM2_HIGH    0x35
#define CMOS_BIOS_BOOTFLAG1      0x38
#define CMOS_BIOS_DISKTRANSFLAG  0x39
#define CMOS_BIOS_BOOTFLAG2      0x3d
#define CMOS_MEM_HIGHMEM_LOW     0x5b
#define CMOS_MEM_HIGHMEM_MID     0x5c
#define CMOS_MEM_HIGHMEM_HIGH    0x5d
#define CMOS_BIOS_SMP_COUNT      0x5f


char read_reg(char reg) {
    out8(CMOS_CTR_REG, NMI_DISABLE_BIT | reg);
    return in8(CMOS_DATA_REG);
}

void write_reg(char reg, char data) {
    out8(CMOS_CTR_REG, NMI_DISABLE_BIT | reg);
    out8(CMOS_DATA_REG, data);
}


#define CURRENT_YEAR        2020                            // Change this each year!

int century_register = 0x00;                                // Set by ACPI table parsing code if possible

static unsigned char second;
static unsigned char minute;
static unsigned char hour;
static unsigned char day;
static unsigned char month;
static unsigned int year;

enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};

int get_update_in_progress_flag() {
      out8(cmos_address, 0x0A);
      return (in8(cmos_data) & 0x80);
}

unsigned char get_RTC_register(int reg) {
      out8(cmos_address, reg);
      return in8(cmos_data);
}

void read_rtc() {
      unsigned char century;
      unsigned char last_second;
      unsigned char last_minute;
      unsigned char last_hour;
      unsigned char last_day;
      unsigned char last_month;
      unsigned char last_year;
      unsigned char last_century;
      unsigned char registerB;

      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates

      while (get_update_in_progress_flag());                // Make sure an update isn't in progress
      second = get_RTC_register(0x00);
      minute = get_RTC_register(0x02);
      hour = get_RTC_register(0x04);
      day = get_RTC_register(0x07);
      month = get_RTC_register(0x08);
      year = get_RTC_register(0x09);
      if(century_register != 0) {
            century = get_RTC_register(century_register);
      }

      do {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;

            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
            if(century_register != 0) {
                  century = get_RTC_register(century_register);
            }
      } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century) );

      registerB = get_RTC_register(0x0B);

      // Convert BCD to binary values if necessary

      if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0) {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }

      // Convert 12 hour clock to 24 hour clock if necessary

      if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }

      // Calculate the full (4-digit) year

      if(century_register != 0) {
            year += century * 100;
      } else {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) year += 100;
      }
}
int time(struct tm *tm) {
    read_rtc();
    if (tm != NULL) {
        tm->tm_sec = second;
        tm->tm_min = minute;
        tm->tm_hour = hour;
        tm->tm_mday = day;
        tm->tm_mon = month;
        tm->tm_year = (int)year;
    }
    return 0;
}
