/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : app_freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "key.h"
#include "iwdg.h"
#include "led.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "ssd1306.h"
#include "eeprom.h"
#include "dw_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// #define __DEBUG_INFO
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
#if configGENERATE_RUN_TIME_STATS == 1
extern uint32_t g_osRuntimeCounter;

#endif
extern uint8_t UART_RX_DATA[256];
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 72 * 4
};
/* Definitions for Flash_LED */
osThreadId_t Flash_LEDHandle;
const osThreadAttr_t Flash_LED_attributes = {
  .name = "Flash_LED",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 80 * 4
};
/* Definitions for Key_Event_Scan */
osThreadId_t Key_Event_ScanHandle;
const osThreadAttr_t Key_Event_Scan_attributes = {
  .name = "Key_Event_Scan",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 100 * 4
};
/* Definitions for Key_Process */
osThreadId_t Key_ProcessHandle;
const osThreadAttr_t Key_Process_attributes = {
  .name = "Key_Process",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 80 * 4
};
/* Definitions for Task_Status */
osThreadId_t Task_StatusHandle;
const osThreadAttr_t Task_Status_attributes = {
  .name = "Task_Status",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 200 * 4
};
/* Definitions for Parse_RX */
osThreadId_t Parse_RXHandle;
const osThreadAttr_t Parse_RX_attributes = {
  .name = "Parse_RX",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 220 * 4
};
/* Definitions for OLED_Display */
osThreadId_t OLED_DisplayHandle;
const osThreadAttr_t OLED_Display_attributes = {
  .name = "OLED_Display",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 200 * 4
};
/* Definitions for DW_Init */
osThreadId_t DW_InitHandle;
const osThreadAttr_t DW_Init_attributes = {
  .name = "DW_Init",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for DW_Main */
osThreadId_t DW_MainHandle;
const osThreadAttr_t DW_Main_attributes = {
  .name = "DW_Main",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 600 * 4
};
/* Definitions for FLASH_LED_Q */
osMessageQueueId_t FLASH_LED_QHandle;
const osMessageQueueAttr_t FLASH_LED_Q_attributes = {
  .name = "FLASH_LED_Q"
};
/* Definitions for KEY_EVENT_SEMA */
osSemaphoreId_t KEY_EVENT_SEMAHandle;
const osSemaphoreAttr_t KEY_EVENT_SEMA_attributes = {
  .name = "KEY_EVENT_SEMA"
};
/* Definitions for UART_RX_SEMA */
osSemaphoreId_t UART_RX_SEMAHandle;
const osSemaphoreAttr_t UART_RX_SEMA_attributes = {
  .name = "UART_RX_SEMA"
};
/* Definitions for KEY_EVENT */
osEventFlagsId_t KEY_EVENTHandle;
const osEventFlagsAttr_t KEY_EVENT_attributes = {
  .name = "KEY_EVENT"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Flash_led_Task(void *argument);
void Key_Event_Scan_Task(void *argument);
void Key_Process_Task(void *argument);
void Task_Status_Task(void *argument);
void Parse_RX_Task(void *argument);
void OLED_Display_Task(void *argument);
extern void DW_Init_Task(void *argument);
extern void DW_Main_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
  g_osRuntimeCounter = 0;
}

__weak unsigned long getRunTimeCounterValue(void)
{
  return g_osRuntimeCounter;
}
/* USER CODE END 1 */

/* USER CODE BEGIN PREPOSTSLEEP */
__weak void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
  /* place for user code */
}

__weak void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
  /* place for user code */
}
/* USER CODE END PREPOSTSLEEP */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of KEY_EVENT_SEMA */
  KEY_EVENT_SEMAHandle = osSemaphoreNew(1, 1, &KEY_EVENT_SEMA_attributes);

  /* creation of UART_RX_SEMA */
  UART_RX_SEMAHandle = osSemaphoreNew(1, 1, &UART_RX_SEMA_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of FLASH_LED_Q */
  FLASH_LED_QHandle = osMessageQueueNew (1, sizeof(LED_Param), &FLASH_LED_Q_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Flash_LED */
  Flash_LEDHandle = osThreadNew(Flash_led_Task, NULL, &Flash_LED_attributes);

  /* creation of Key_Event_Scan */
  Key_Event_ScanHandle = osThreadNew(Key_Event_Scan_Task, NULL, &Key_Event_Scan_attributes);

  /* creation of Key_Process */
  Key_ProcessHandle = osThreadNew(Key_Process_Task, NULL, &Key_Process_attributes);

  /* creation of Task_Status */
  Task_StatusHandle = osThreadNew(Task_Status_Task, NULL, &Task_Status_attributes);

  /* creation of Parse_RX */
  Parse_RXHandle = osThreadNew(Parse_RX_Task, NULL, &Parse_RX_attributes);

  /* creation of OLED_Display */
  OLED_DisplayHandle = osThreadNew(OLED_Display_Task, NULL, &OLED_Display_attributes);

  /* creation of DW_Init */
  DW_InitHandle = osThreadNew(DW_Init_Task, NULL, &DW_Init_attributes);

  /* creation of DW_Main */
  DW_MainHandle = osThreadNew(DW_Main_Task, NULL, &DW_Main_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of KEY_EVENT */
  KEY_EVENTHandle = osEventFlagsNew(&KEY_EVENT_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  /* Infinite loop */
  for (;;)
  {
    HAL_IWDG_Refresh(&hiwdg);
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Flash_led_Task */
/**
 * @brief Function implementing the Flash_LED thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Flash_led_Task */
void Flash_led_Task(void *argument)
{
  /* USER CODE BEGIN Flash_led_Task */
  /* Infinite loop */
  for (LED_Param led_param;;)
  {
    if (pdTRUE == xQueueReceive(FLASH_LED_QHandle, (void *)&led_param, portMAX_DELAY))
      Flash_LED(led_param.led, led_param.interval, led_param.count, led_param.cond);
    osDelay(50);
  }
  /* USER CODE END Flash_led_Task */
}

/* USER CODE BEGIN Header_Key_Event_Scan_Task */
/**
 * @brief Function implementing the Key_Event_Scan thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Key_Event_Scan_Task */
void Key_Event_Scan_Task(void *argument)
{
  /* USER CODE BEGIN Key_Event_Scan_Task */
  /* Infinite loop */
  for (;;)
  {
    if (pdTRUE == xSemaphoreTake(KEY_EVENT_SEMAHandle, portMAX_DELAY))
      KEY_Scan();
    osDelay(50);
  }
  /* USER CODE END Key_Event_Scan_Task */
}

/* USER CODE BEGIN Header_Key_Process_Task */
/**
 * @brief Function implementing the Key_Process thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Key_Process_Task */
void Key_Process_Task(void *argument)
{
  /* USER CODE BEGIN Key_Process_Task */
  /* Infinite loop */
  for (;;)
  {
    for (int i = 0; i < KEYS; i++)
      KEY_Process(i);
    osDelay(pdMS_TO_TICKS(100));
  }
  /* USER CODE END Key_Process_Task */
}

/* USER CODE BEGIN Header_Task_Status_Task */
/**
 * @brief Function implementing the Task_Status thread.
 * @param argument: Not used
 * @retval None
 */
#if configGENERATE_RUN_TIME_STATS == 1
char tasks_buf[512];
#endif
/* USER CODE END Header_Task_Status_Task */
void Task_Status_Task(void *argument)
{
  /* USER CODE BEGIN Task_Status_Task */

  /* Infinite loop */
  for (;;)
  {
#ifdef __DEBUG_INFO
    memset(tasks_buf, 0, 512);
    printf("任务名称\t运行状态\t优先级\t剩余堆栈\t任务序号\r\n");
    printf("-------------------------------------------------------\r\n");
    vTaskList(tasks_buf);
    strcat((char *)tasks_buf, "-------------------------------------------------------\r\n");
    strcat((char *)tasks_buf, "B: 阻塞, R: 就绪, D: 删除, S: 暂停\r\n\r\n");
    printf("%s", tasks_buf);
    memset(tasks_buf, 0, 512);
    vTaskGetRunTimeStats(tasks_buf);
    printf("\r\n任务名称\t运行计数\t\t使用率\r\n");
    printf("----------------------------------------------\r\n");
    strcat((char *)tasks_buf, "---------------------结 束---------------------\r\n");
    printf("%s", tasks_buf);
    printf("剩余内存: %d bytes.\r\n\r\n", xPortGetFreeHeapSize());
#endif
    osDelay(3000);
  }

  /* USER CODE END Task_Status_Task */
}

/* USER CODE BEGIN Header_Parse_RX_Task */
/**
 * @brief Function implementing the Parse_RX thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Parse_RX_Task */
void Parse_RX_Task(void *argument)
{
  /* USER CODE BEGIN Parse_RX_Task */
  /* Infinite loop */
  for (;;)
  {
    if (pdTRUE == xSemaphoreTake(UART_RX_SEMAHandle, portMAX_DELAY))
    {

      uint8_t rx_len = strlen((char *)UART_RX_DATA);
      LED_Param flashParam;
      flashParam.led = LED_YELLOW;
      flashParam.cond = LIGHT_OFF;
      flashParam.count = rx_len;
      flashParam.interval = 15;
      xQueueSend(FLASH_LED_QHandle, (void *)&flashParam, pdMS_TO_TICKS(500));

      printf("Received from uart: %s\r\n", UART_RX_DATA);
      // printf("RX Length: %d\r\n", rx_len);
      taskENTER_CRITICAL();
      Parse_RX();
      memset(UART_RX_DATA, 0, 256);
      taskEXIT_CRITICAL();
    }
    osDelay(1);
  }
  /* USER CODE END Parse_RX_Task */
}

/* USER CODE BEGIN Header_OLED_Display_Task */
/**
 * @brief Function implementing the OLED_Display thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_OLED_Display_Task */
void OLED_Display_Task(void *argument)
{
  /* USER CODE BEGIN OLED_Display_Task */
  /* Infinite loop */
  LED_Param flashParam;
  for (uint8_t dead_time = 0;;)
  {
    // 进入设置模式后，无操作大约4秒，自动退出并保存参数
    if (dead_time > 18)
    {
      xEventGroupClearBits(KEY_EVENTHandle, EVENTBIT_KEY_MINUS_LONGPRESS | EVENTBIT_KEY_PLUS_LONGPRESS);
      save_UWB_User_Param();
      flashParam.led = LED_YELLOW;
      flashParam.cond = LIGHT_OFF;
      flashParam.count = 4;
      flashParam.interval = 30;
      xQueueSend(FLASH_LED_QHandle, (void *)&flashParam, pdMS_TO_TICKS(500));
      dead_time = 0;
    }
    EventBits_t event = xEventGroupWaitBits(KEY_EVENTHandle,
                                            EVENTBIT_KEY_PLUS_LONGPRESS | EVENTBIT_KEY_MINUS_LONGPRESS,
                                            pdFALSE, pdFALSE, portMAX_DELAY);
    EventBits_t event_click = xEventGroupWaitBits(KEY_EVENTHandle,
                                                  EVENTBIT_KEY_MINUS_CLICK | EVENTBIT_KEY_PLUS_CLICK,
                                                  pdTRUE, pdFALSE, 0);

    // 加号键长按，进入设备模式设置（基站还是标签）
    if ((event & EVENTBIT_MASK_KEY_PLUS_LONGPRESS) == EVENTBIT_KEY_PLUS_LONGPRESS)
    {
      // 闪烁在设置状态的参数显示
      if (dead_time % 2)
      {
        sprintf(row1_str, "        0x%04X", uwb_dup.device_address);
        LCD_DISPLAY(0, 16, row1_str);
      }
      else
      {
        LCD_DISPLAY_UWB();
      }
      if ((event_click & EVENTBIT_MASK_KEY_PLUS_CLICK) == EVENTBIT_KEY_PLUS_CLICK)
      {
        uwb_dup.device_mode ^= 1;
        dead_time = 1;
      }
      else if ((event_click & EVENTBIT_MASK_KEY_MINUS_CLICK) == EVENTBIT_KEY_MINUS_CLICK)
      {
        uwb_dup.device_mode ^= 1;
        dead_time = 1;
      }
    }
    // 减号键长按，进入设备地址设置
    else if ((event & EVENTBIT_MASK_KEY_MINUS_LONGPRESS) == EVENTBIT_KEY_MINUS_LONGPRESS)
    {
      if (dead_time % 2)
      {
        if (uwb_dup.device_mode)
          sprintf(row1_str, "Anchor        ");
        else
          sprintf(row1_str, "TAG           ");
        LCD_DISPLAY(0, 16, row1_str);
      }
      else
      {
        LCD_DISPLAY_UWB();
      }
      if ((event_click & EVENTBIT_MASK_KEY_PLUS_CLICK) == EVENTBIT_KEY_PLUS_CLICK)
      {
        dead_time = 1;
        uwb_dup.device_address++;
      }
      else if ((event_click & EVENTBIT_MASK_KEY_MINUS_CLICK) == EVENTBIT_KEY_MINUS_CLICK)
      {
        dead_time = 1;
        uwb_dup.device_address--;
      }
    }
    dead_time++;
    osDelay(200);
  }
  /* USER CODE END OLED_Display_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

