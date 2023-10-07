#include "dw_app.h"
#include "ssd1306.h"
#include "cmsis_os.h"
#include "iwdg.h"
#include "eeprom.h"
#include "stdio.h"

#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385

extern dwt_txconfig_t txconfig_options;
extern osThreadId_t defaultTaskHandle;
extern osThreadId_t DW_MainHandle;

uint32 inittestapplication(void);

volatile uint8_t s1switch = 0;
uint16_t instance_anchaddr = 0;
uint8_t dr_mode = 0;
uint8_t tagaddr, ancaddr;
uint8_t instance_mode = ANCHOR;
uint8_t max_tag_num = 10;

#define LCD_BUFF_LEN (20)
char lcd_data[LCD_BUFF_LEN];

// static dwt_config_t config = {
//     5,                /* 信道号. Channel number. */
//     DWT_PLEN_512,     /* Preamble length. Used in TX only. */
//     DWT_PAC16,        /* Preamble acquisition chunk size. Used in RX only. */
//     4,                /* Tx前导码. TX preamble code. Used in TX only. */
//     4,                /* Rx前导码. RX preamble code. Used in RX only. */
//     0,                /* 帧分隔符模式. 0 to use standard 8 symbol SFD, 1 to use non-standard 8 symbol, 2 for non-standard 16 symbol SFD and 3 for 4z 8 symbol SDF type */
//     DWT_BR_850K,      /* 数据速率. Data rate. */
//     DWT_PHRMODE_STD,  /* 物理层头模式. PHY header mode. */
//     DWT_PHRRATE_STD,  /* 物理层头速率. PHY header rate. */
//     (513 + 8 - 16),   /* 帧分隔符超时. SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
//     DWT_STS_MODE_OFF, /* STS模式. STS disabled */
//     DWT_STS_LEN_64,   /* STS长度. STS length see allowed values in Enum dwt_sts_lengths_e */
//     DWT_PDOA_M0       /* PDOA mode off */
// };

instanceConfig_t chConfig[2] = {
    // mode 1 - SW: 2 off 850K ch5
    {
        .channelNumber = 5,          // channel
        .preambleCode = 4,           // preambleCode
        .pulseRepFreq = DWT_PRF_16M, // prf
        .dataRate = DWT_BR_850K,     // datarate
        .preambleLen = DWT_PLEN_512, // preambleLength
        .pacSize = DWT_PAC16,        // pacSize
        .nsSFD = 0,                  // non-standard SFD
        .sfdTO = (513 + 8 - 16)      // SFD timeout		preamble length + 1 + SFD length – PAC size
    },
    // mode 2 - SW: 2 on 6.8M ch5
    {
        .channelNumber = 5,          // channel
        .preambleCode = 4,           // preambleCode
        .pulseRepFreq = DWT_PRF_16M, // prf
        .dataRate = DWT_BR_6M8,      // datarate
        .preambleLen = DWT_PLEN_128, // preambleLength
        .pacSize = DWT_PAC8,         // pacSize
        .nsSFD = 0,                  // non-standard SFD
        .sfdTO = (129 + 8 - 8)       // SFD timeout		preamble length + 1 + SFD length – PAC size
    },
};

sfConfig_t sfConfig[2] = {
// mode 1 - SW: 2 off 110K ch2 4tags 112ms
#define SLOT_TIME_850K 80
#define SLOT_TIME_6M8 10
    {
        .slotDuration_ms = (SLOT_TIME_850K),             // slot duration in milliseconds (NOTE: the ranging exchange must be able to complete in this time
        .numSlots = (MAX_TAG_850K),                      // number of slots in the superframe (8 tag slots and 2 used for anchor to anchor ranging),
        .sfPeriod_ms = (MAX_TAG_850K * SLOT_TIME_850K),  // in ms => 280ms frame means 3.57 Hz location rate
        .tagPeriod_ms = (MAX_TAG_850K * SLOT_TIME_850K), // tag period in ms (sleep time + ranging time)
        .pollTxToFinalTxDly_us = (60000)                 // 13500 poll to final delay in microseconds (needs to be adjusted according to lengths of ranging frames)
    },
#if (DISCOVERY == 1)
    // mode 2 - SW: 2 on
    {
        .slotDuration_ms = (10),        // slot duration in milliseconds (NOTE: the ranging exchange must be able to complete in this time
                                        // e.g. tag sends a poll, 4 anchors send responses and tag sends the final + processing time
        .numSlots = (100),              // number of slots in the superframe (98 tag slots and 2 used for anchor to anchor ranging),
        .sfPeriod_ms = (10 * 100),      // in ms => 1000 ms frame means 1 Hz location rate
        .tagPeriod_ms = (10 * 100),     // tag period in ms (sleep time + ranging time)
        .pollTxToFinalTxDly_us = (2500) // poll to final delay in microseconds (needs to be adjusted according to lengths of ranging frames)

    }
#else
    // mode 2 - SW: 2 on 6.8M ch2 15tags 150ms
    {
        .slotDuration_ms = (SLOT_TIME_6M8),            // slot duration in milliseconds (NOTE: the ranging exchange must be able to complete in this time
        .numSlots = (MAX_TAG_68M),                     // number of slots in the superframe (8 tag slots and 2 used for anchor to anchor ranging),
        .sfPeriod_ms = (MAX_TAG_68M * SLOT_TIME_6M8),  // in ms => 100 ms frame means 10 Hz location rate
        .tagPeriod_ms = (MAX_TAG_68M * SLOT_TIME_6M8), // tag period in ms (sleep time + ranging time)
        .pollTxToFinalTxDly_us = (2500)                // poll to final delay in microseconds (needs to be adjusted according to lengths of ranging frames)

    }
#endif
};

void addressconfigure(uint8 mode)
{
    uint16 instAddress;
    instance_anchaddr = rom_getDeviceAddr();

    if (mode == ANCHOR)
    {
        instAddress = GATEWAY_ANCHOR_ADDR | instance_anchaddr;
    }
    else
    {
        instAddress = instance_anchaddr;
    }
    instance_set_16bit_address(instAddress);
}

uint8_t decarangingmode(void)
{
    uint8_t mode = 0;

    return mode;
}

void DW_Init_Task(void *argument)
{
    port_DisableEXT_IRQ();
    // port_set_dw_ic_spi_fastrate();
    reset_DWIC();
    osDelay(2);

    while (!dwt_checkidlerc())
    {
        osDelay(1);
    };

    if (inittestapplication() != (uint32)-1)
    {
        LCD_DISPLAY(0, 32, "DW Config Success!");
        xTaskNotifyGive(defaultTaskHandle); // 初始化成功通知启动看门狗和LED闪烁任务
        xTaskNotifyGive(DW_MainHandle);     // 通知启动DW主任务
        port_EnableEXT_IRQ();
        osDelay(2000);
        LCD_DISPLAY(0, 32, "                  ");
        vTaskDelete(NULL);
    }
    else
        LCD_DISPLAY(0, 32, "DW Config Fail!");
}

void DW_Main_Task(void *argument)
{
    /* int rx = 0;
    int toggle = 0;

    uint64_t printLCDTWRReports = 0;
    uint64_t NanTWRReports = 0; */

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    for (;;)
    {
        HAL_IWDG_Refresh(&hiwdg);
        instance_data_t *inst = instance_get_local_structure_ptr();
        int monitor_local = inst->monitor;
        int txdiff = (portGetTickCnt() - inst->timeofTx);
        instance_mode = instance_get_role();

        if (instance_mode == TAG) // 根据角色进入相应的状态机流程
        {
            tag_run();
        }
        else
        {
            anch_run();
        }

        if ((monitor_local == 1) && (txdiff > inst->slotDuration_ms))
        {
            inst->wait4ack = 0;
            if (instance_mode == TAG)
            {
                tag_process_rx_timeout(inst);
            }
            else
            {
                dwt_forcetrxoff();
                inst->AppState = TA_RXE_WAIT;
            }
            inst->monitor = 0;
        }

        /* rx = instance_newrange();
        if (rx != TOF_REPORT_NUL)
        {
            NanTWRReports = 0;
            int l = 0, r = 0, aaddr, taddr;
            int rangeTime, valid;

            aaddr = instance_newrangeancadd() & 0xf;
#if (DISCOVERY == 1)
            taddr = instance_newrangetagadd() & 0xff;
#else
            taddr = instance_newrangetagadd() & 0xff;
#endif
            rangeTime = instance_newrangetim() & 0xffffffff;

#if (OLED == 1)

            if (printLCDTWRReports + 1500 <= portGetTickCnt())
            {
                // 每1.5S更新一次测距数据
                if (instance_mode == ANCHOR)
                {
                    int b = 0;
                    double rangetotag = instance_get_tagdist(toggle);

                    while (((int)(rangetotag * 1000)) == 0)
                    {
                        if (b > (max_tag_num - 1))
                            break;

                        toggle++;
                        if (toggle >= max_tag_num)
                            toggle = 0;

                        rangetotag = instance_get_tagdist(toggle);
                        b++;
                    }
                    sprintf((char *)&lcd_data[0], "A%d-T%d: %3.2f m  ", ancaddr, toggle, rangetotag);
                    LCD_DISPLAY(0, 48, lcd_data);
                    // sprintf((char *)&RxPower, "T%d: %3.1f dBm   ", toggle, inst->rxPower[0]);
                    // LCD_DISPLAY(0, 16, RxPower);
                    toggle++;
                    if (toggle >= max_tag_num)
                        toggle = 0;
                }
                else if (instance_mode == TAG)
                {
                    int b = 0;
                    double rangetotag = instance_get_idist(toggle);

                    while (((int)(rangetotag * 1000)) == 0)
                    {
                        if (b > (MAX_ANCHOR_LIST_SIZE - 1))
                            break;

                        toggle++;
                        if (toggle >= MAX_ANCHOR_LIST_SIZE)
                            toggle = 0;

                        rangetotag = instance_get_idist(toggle);
                        b++;
                    }
#if (DISCOVERY == 1)
                    sprintf((char *)&lcd_data[0], "T%d A%d: %3.2f m", taddr, toggle, instance_get_idist(toggle));
#else
                    sprintf((char *)&lcd_data[0], "T%d-A%d: %3.2f m  ", tagaddr, toggle, instance_get_idist(toggle));
                    // sprintf((char *)&RxPower[0], "%3.1f dBm     ", inst->rxPower[toggle]);
#endif
                    LCD_DISPLAY(0, 48, lcd_data);
                    // LCD_DISPLAY(52, 48, RxPower);
                    toggle++;
                    if (toggle >= MAX_ANCHOR_LIST_SIZE)
                        toggle = 0;
                }
                printLCDTWRReports = portGetTickCnt();
            }
#endif
        } */
        // osDelay(1);
    }
}

uint32 inittestapplication(void)
{
    uint32 devID;
    int result;

    instance_mode = rom_getDeviceMode();

    result = instance_init(instance_mode); // 设置工作模式
    if (0 > result)
    {
        return (-1);
    }

    port_set_dw_ic_spi_fastrate();   // SPI高速模式
    devID = instance_readdeviceid(); // 读取设备ID

    if (DWT_C0_DEV_ID != devID) // SPI异常，或硬件故障
    {
        return (-1);
    }

    addressconfigure(instance_mode); // 设置设备短地址
    dr_mode = decarangingmode();     // 读取拨码开关第2位，on=6.8M,15tags,28*4ms   off=110K,4tags,10*15ms
    instance_config(&chConfig[dr_mode], &sfConfig[dr_mode]);
    if (dr_mode == 1)
    {
        max_tag_num = MAX_TAG_68M;
    }
    else
    {
        max_tag_num = MAX_TAG_850K;
    }
    return devID;
}
