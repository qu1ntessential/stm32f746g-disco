#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "FatFsWrapper.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "stm32f7xx.h"
#include "stm32746g_discovery.h"

#include "usart.h"
#include "uart_log.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

//#include "DFPlayer.hpp"

#define LED_GPIO_Port GPIOI
#define LED_Pin       GPIO_PIN_13

#define UART_CLI_TASK_STACK_SIZE 1024

extern TaskHandle_t UartCliTaskHandle;
extern volatile uint8_t UartCliAlive;

/**
 * @brief Задача FreeRTOS, обрабатывающая UART CLI
 * @param argument
 */
void UartCliThread(void *argument);

void UART_CLI_Init(void);

#ifdef __cplusplus
}
#endif

#endif // COMMANDS_HPP
