#ifndef OSTASKS_H
#define OSTASKS_H

#include <cstdio>
#include <cstring>

#include "stm32f7xx.h"

#include "usart.h"
#include "uart_log.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "FatFsWrapper.hpp"
#include "ESG.hpp"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"

#include "lvgl.h"
#include "lvgl_port_tft.h"
#include "lvgl_port_touchpad.h"

#include "ui.h"
#include "ui_con.h"
#include "vars.h"
#include "actions.h"

#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dcmi.h"
#include "dma2d.h"
#include "eth.h"
#include "i2c.h"
#include "ltdc.h"
#include "quadspi.h"
#include "rtc.h"
#include "sai.h"
#include "sdmmc.h"
#include "spdifrx.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

void FreeRTOS_Resources_Init();

/**
 * @brief Поток для обработки UI
 * @note Все изменения интерфейса должны осуществляться в этом потоке!
 * @param argument
 */
void LvglThread(void *argument);

/**
 * @brief Поток для печати отладочных сообщений в UART (используя очередь FreeRTOS)
 * @param argument
 */
void UartThread(void *argument);

/**
 * @brief
 * @param argument
 */
void TwiThread(void *argument);

/**
 * @brief
 * @param argument
 */
void Task4Thread(void *argument);

#endif // OSTASKS_H
