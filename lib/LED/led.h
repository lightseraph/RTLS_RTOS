#ifndef __LED_H
#define __LED_H

#include "main.h"

typedef enum
{
    LED_RED,
    LED_YELLOW
} LED_TYPE;

typedef enum
{
    LIGHT_ON = 0,
    LIGHT_OFF,
    FOLLOW_PREVIOUS,
} LED_AFTER_FLASH;

typedef struct
{
    LED_TYPE led;
    u16 interval;
    u16 count;
    LED_AFTER_FLASH cond;
} LED_Param;

void Flash_LED(LED_TYPE led, u16 interval, u8 count, LED_AFTER_FLASH cond);

#endif