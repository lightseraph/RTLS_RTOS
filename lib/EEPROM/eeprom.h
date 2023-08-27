#ifndef __EEPROM_H
#define __EEPROM_H
#include "24cxx.h"

#define EEPROM_DATAFORMAT (0xB1)

#define RF_PARAM_BASEADDR (0x0FE0)
#define LOCAL_ID_ADDR (RF_PARAM_BASEADDR + 0x0)
#define FREQ_ADDR (RF_PARAM_BASEADDR + 0x1)

#define UWB_PARAM_BASEADDR (0x0001)
#define UWB_PARAM_DEVICEADDR (UWB_PARAM_BASEADDR + 0x0)
#define UWB_PARAM_TA_KAL (UWB_PARAM_BASEADDR + 0x2)

typedef struct
{
    uint8_t device_mode;
    uint16_t device_address;
} UWB_Device_User_Param;

extern UWB_Device_User_Param uwb_dup;

void Write_UWB_Default_Param(void);
uint8_t rom_getDeviceMode(void);
uint16_t rom_getDeviceAddr(void);
uint8_t rom_getKalFilter(void);
void rom_setDeviceMode(uint8_t mode);
void init_UWB_User_Param(void);
void rom_setDeviceAddr(void);
void save_UWB_User_Param(void);

#endif