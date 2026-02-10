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

#include "FatFsWrapper.hpp"

#include "FreeRTOS_CLI.h"
#include "Commands.hpp"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_audio.h"

#include "lvgl.h"
#include "lvgl_port_tft.h"
#include "lvgl_port_touchpad.h"

#include "ui.h"
#include "vars.h"
#include "actions.h"

/**
 * @brief Функция инициализации FreeRTOS ресурсов (заадчи, очереди, семафоры и т.д.)
 */
void FreeRTOS_Resources_Init();