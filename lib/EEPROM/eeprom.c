#include "eeprom.h"
#include "delay.h"

UWB_Device_User_Param uwb_dup;

void Write_UWB_Default_Param(void)
{
    uint8_t temp = 0;
    // AT24CXX_WriteOneByte(LOCAL_ID_ADDR, temp); // 默认出厂设置id索引号
    temp = 2;
    AT24CXX_WriteOneByte(UWB_PARAM_TA_KAL, temp); // 1:基站，0：标签;    0010
    temp = 1;
    // AT24CXX_WriteOneByte(FREQ_ADDR, temp); // 默认出厂设置频点索引号
    uint16_t addr = 0x0002; // 标签地址
    AT24CXX_WriteLenByte(UWB_PARAM_DEVICEADDR, (u32)addr, 2);

    delay_ms(50);
}

uint8_t rom_getDeviceMode(void)
{
    uint8_t temp = AT24CXX_ReadOneByte(UWB_PARAM_TA_KAL);
    return (temp & 0x01);
}

uint16_t rom_getDeviceAddr(void)
{
    uint16_t temp = (uint16_t)AT24CXX_ReadLenByte(UWB_PARAM_DEVICEADDR, 2);
    return temp;
}

uint8_t rom_getKalFilter(void)
{
    uint8_t temp = AT24CXX_ReadOneByte(UWB_PARAM_TA_KAL);
    return (temp & 0x02);
}

void rom_setDeviceMode(uint8_t mode)
{
    uint8_t temp = AT24CXX_ReadOneByte(UWB_PARAM_TA_KAL);
    if (mode)
        temp |= mode;
    else
        temp &= ~1;
    AT24CXX_WriteOneByte(UWB_PARAM_TA_KAL, temp);
}

void init_UWB_User_Param(void)
{
    uwb_dup.device_address = rom_getDeviceAddr();
    uwb_dup.device_mode = rom_getDeviceMode();
}

void rom_setDeviceAddr(void)
{
    AT24CXX_WriteLenByte(UWB_PARAM_DEVICEADDR, (u32)uwb_dup.device_address, 2);
}

void save_UWB_User_Param(void)
{
    rom_setDeviceAddr();
    rom_setDeviceMode(uwb_dup.device_mode);
}
