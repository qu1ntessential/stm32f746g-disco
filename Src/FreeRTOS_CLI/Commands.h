#ifndef COMMANDS_H
#define COMMANDS_H

#include "stdio.h"
#include "string.h"

#include "stm32f7xx.h"

#include "stm32746g_discovery.h"

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"

extern const CLI_Command_Definition_t xLedCommand;
extern const CLI_Command_Definition_t xTasksCommand;

#endif // COMMANDS_H
