#pragma once

#include "stm32f7xx.h"

#include "dac.h"
#include "usart.h"
#include "i2c.h"
#include "tim.h"
#include "iwdg.h"
#include "uart_log.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "I2C.hpp"
#include "FatFsWrapper.hpp"
#include "QSPI.hpp"
#include "DacDriver.hpp"

#include "FreeRTOS_CLI.h"
#include "Commands.hpp"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_audio.h"

/*
#include "lvgl.h"
#include "lvgl_port_tft.h"
#include "lvgl_port_touchpad.h"

#include "ui.h"
#include "vars.h"
#include "actions.h"
 */

extern TaskHandle_t WatchdogTaskHandle;
extern TaskHandle_t LvglTaskHandle;
extern TaskHandle_t UartTaskHandle;
extern TaskHandle_t TwiTaskHandle;
extern TaskHandle_t Task4Handle;

/**
 * @brief Функция инициализации FreeRTOS ресурсов (заадчи, очереди, семафоры и т.д.)
 */
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

/**
 * @brief
 * @param argument
 */
void cliThread(void *argument);