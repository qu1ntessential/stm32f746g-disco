#ifndef COMMANDS_H
#define COMMANDS_H

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
#include "FreeRTOS_CLI.h"

#define LED_GPIO_Port GPIOI
#define LED_Pin       GPIO_PIN_13

#define UART_CLI_TASK_STACK_SIZE 1024

extern const CLI_Command_Definition_t xGpioCommand;
extern const CLI_Command_Definition_t xTasksCommand;

extern TaskHandle_t UartCliTaskHandle;
extern StaticTask_t UartCliTaskBuffer;
extern StackType_t UartCliTaskStack[UART_CLI_TASK_STACK_SIZE];

/**
 * @brief Задача FreeRTOS, обрабатывающая UART CLI
 * @param argument
 */
void UartCliThread(void *argument);

void UART_CLI_Init(void);

#ifdef __cplusplus
}
#endif

#endif // COMMANDS_H
