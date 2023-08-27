#include "led.h"
#include "delay.h"

void Flash_LED(LED_TYPE led, u16 interval, u8 count, LED_AFTER_FLASH cond)
{
    for (int i = 0; i < count * 2; i++)
    {
        if (led == LED_RED)
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        else if (led == LED_YELLOW)
            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

        delay_ms(interval);
    }
    if (cond != FOLLOW_PREVIOUS)
    {
        if (led == LED_RED)
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, (GPIO_PinState)cond);
        else if (led == LED_YELLOW)
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, (GPIO_PinState)cond);
    }
}