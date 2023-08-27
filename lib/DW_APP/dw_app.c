#include "dw_app.h"
#include "port.h"
#include "deca_regs.h"
#include "ssd1306.h"
#include "cmsis_os.h"

#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385

extern dwt_txconfig_t txconfig_options;

static dwt_config_t config = {
    5,                /* 信道号. Channel number. */
    DWT_PLEN_512,     /* Preamble length. Used in TX only. */
    DWT_PAC32,        /* Preamble acquisition chunk size. Used in RX only. */
    4,                /* Tx前导码. TX preamble code. Used in TX only. */
    4,                /* Rx前导码. RX preamble code. Used in RX only. */
    2,                /* 帧分隔符模式. 0 to use standard 8 symbol SFD, 1 to use non-standard 8 symbol, 2 for non-standard 16 symbol SFD and 3 for 4z 8 symbol SDF type */
    DWT_BR_850K,      /* 数据速率. Data rate. */
    DWT_PHRMODE_STD,  /* 物理层头模式. PHY header mode. */
    DWT_PHRRATE_STD,  /* 物理层头速率. PHY header rate. */
    (513 + 16 - 32),  /* 帧分隔符超时. SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
    DWT_STS_MODE_OFF, /* STS模式. STS disabled */
    DWT_STS_LEN_64,   /* STS长度. STS length see allowed values in Enum dwt_sts_lengths_e */
    DWT_PDOA_M0       /* PDOA mode off */
};

void DW_Init_Task(void *argument)
{
    port_set_dw_ic_spi_fastrate();
    reset_DWIC();
    osDelay(2);

    while (!dwt_checkidlerc())
    {
        osDelay(1);
    };

    if (dwt_initialise(DWT_DW_INIT) == DWT_ERROR)
    {
        LCD_DISPLAY(0, 32, "DW INIT Error!");
    }
    else
        LCD_DISPLAY(0, 32, "DW INIT Success!");
    osDelay(1000);
    if (!dwt_configure(&config))
        LCD_DISPLAY(0, 32, "DW Config Success!");

    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);

    dwt_setleds(DWT_LEDS_ENABLE | DWT_LEDS_INIT_BLINK);
    dwt_setlnapamode(DWT_LNA_ENABLE | DWT_PA_ENABLE);
    osDelay(2000);
    LCD_DISPLAY(0, 32, "                  ");
    vTaskDelete(NULL);
}

void dw_tag_init(void)
{
}

void dw_anchor_init(void)
{
}
