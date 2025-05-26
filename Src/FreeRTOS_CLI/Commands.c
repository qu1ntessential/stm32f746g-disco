#include "Commands.h"

static TaskHandle_t UartCliTaskHandle = NULL;
static StaticTask_t UartCliTaskBuffer;
static StackType_t UartCliTaskStack[UART_CLI_TASK_STACK_SIZE];

static BaseType_t prvClearCommand(char *pcWriteBuffer,
                                  size_t xWriteBufferLen,
                                  const char *pcCommandString);

static BaseType_t prvResetCommand(char *pcWriteBuffer,
                                  size_t xWriteBufferLen,
                                  const char *pcCommandString);

static BaseType_t prvGpioCommand(char *pcWriteBuffer,
                                 size_t xWriteBufferLen,
                                 const char *pcCommandString);

static BaseType_t prvTasksCommand(char *pcWriteBuffer,
                                  size_t xWriteBufferLen,
                                  const char *pcCommandString);

static BaseType_t prvDfpCommand(char *pcWriteBuffer,
                                size_t xWriteBufferLen,
                                const char *pcCommandString);

static BaseType_t prvFlashCommand(char *pcWriteBuffer,
                                size_t xWriteBufferLen,
                                const char *pcCommandString);

static const CLI_Command_Definition_t xClearCommand = {
        "clear",
        "clear:\r\n\tClears the entire display\r\n\n",
        prvClearCommand,
        0
};

static const CLI_Command_Definition_t xResetCommand = {
        "reset",
        "reset:\r\n\tPerform a software reset of the MCU\r\n\n",
        prvResetCommand,
        0
};

static const CLI_Command_Definition_t xGpioCommand = {
        "gpio",                            // Имя команды (должно быть в нижнем регистре)
        "gpio:\r\n\t"
        "gpio [A..I] [0..15] [set/reset/read]\r\n\n", // Справка
        prvGpioCommand,             // Функция-обработчик
        3                     // Ожидаемое количество параметров
};

static const CLI_Command_Definition_t xTasksCommand = {
        "tasks", // Имя команды
        "tasks:\r\n\tList all running tasks\r\n"
        "\tFormat: <Name> <State> <Priority> <Runtime>\r\n"
        "\tStates: R-Running, Y-Ready, B-Blocked, S-Suspended, D-Deleted\r\n\n",
        prvTasksCommand,
        0 // Без параметров
};

static const CLI_Command_Definition_t xDfpCommand = {
        "dfp", // Имя команды
        "dfp:\r\n\tDFPlayer control commands:\r\n"
        "\tdfp play [0..999]\r\n"
        "\tdfp volume [0..15]\r\n"
        "\tdfp next\r\n"
        "\tdfp prev\r\n"
        "\tdfp pause\r\n"
        "\tdfp resume\r\n\n",
        prvDfpCommand,
        -1
};

static const CLI_Command_Definition_t xFlashCommand = {
        "flash", // Имя команды
        "flash:\r\n\tShow flash data\r\n",
        prvFlashCommand,
        1
};

static GPIO_TypeDef *get_gpio_port(char portChar) {
    switch (portChar) {
        case 'A':
            return GPIOA;
        case 'B':
            return GPIOB;
        case 'C':
            return GPIOC;
        case 'D':
            return GPIOD;
        case 'E':
            return GPIOE;
        case 'F':
            return GPIOF;
        case 'G':
            return GPIOG;
        case 'H':
            return GPIOH;
        case 'I':
            return GPIOI;
        default:
            return NULL;
    }
}

static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    (void) pcCommandString;  // Неиспользуемый параметр

    // Отправляем ANSI-код очистки экрана
    snprintf(pcWriteBuffer, xWriteBufferLen, "\x1B[2J\x1B[H");
    return pdFALSE;
}

static BaseType_t prvResetCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    (void) pcCommandString;
    snprintf(pcWriteBuffer, xWriteBufferLen, "System is resetting...\r\n");

    // Небольшая задержка для завершения вывода
    for (volatile int i = 0; i < 100000; i++);

    // Сброс через системный контроллер
    NVIC_SystemReset();

    return pdFALSE;
}

static BaseType_t prvGpioCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    BaseType_t xParameterStringLength;
    const char *pcPortParam = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);
    const char *pcPinParam = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength);
    const char *pcStateParam = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParameterStringLength);

    if (!pcPortParam || !pcPinParam || !pcStateParam) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Missing parameters.\r\n");
        return pdFALSE;
    }

    // Port
    char portChar = pcPortParam[0];
    GPIO_TypeDef *GPIOx = get_gpio_port(portChar);
    if (!GPIOx) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Invalid GPIO port '%c'\r\n", portChar);
        return pdFALSE;
    }

    // Pin
    char pinStr[4] = {0};
    strncpy(pinStr, pcPinParam, xParameterStringLength > 3 ? 3 : xParameterStringLength);
    uint32_t pin = (uint32_t) atoi(pinStr);
    if (pin > 15) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Invalid GPIO pin '%u'\r\n", pin);
        return pdFALSE;
    }

    uint32_t pinMask = (1 << pin);

    // State
    char action[10] = {0};
    strncpy(action, pcStateParam, xParameterStringLength > 9 ? 9 : xParameterStringLength);
    action[xParameterStringLength] = '\0';

    if (strcasecmp(action, "set") == 0) {
        HAL_GPIO_WritePin(GPIOx, pinMask, GPIO_PIN_SET);
        snprintf(pcWriteBuffer, xWriteBufferLen, "P%c%lu set to HIGH\r\n", portChar, pin);
    } else if (strcasecmp(action, "reset") == 0) {
        HAL_GPIO_WritePin(GPIOx, pinMask, GPIO_PIN_RESET);
        snprintf(pcWriteBuffer, xWriteBufferLen, "P%c%lu set to LOW\r\n", portChar, pin);
    } else if (strcasecmp(action, "read") == 0) {
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIOx, pinMask);
        snprintf(pcWriteBuffer, xWriteBufferLen, "P%c%lu is %s\r\n", portChar, pin,
                 state == GPIO_PIN_SET ? "HIGH" : "LOW");
    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Invalid action '%s'\r\n", action);
    }

    return pdFALSE;
}

static BaseType_t prvTasksCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // Временный буфер для сырых данных
    static char rawBuffer[2048];  // Достаточно большой буфер

    // Получаем список задач
    vTaskList(rawBuffer);

    // Заголовок
    size_t len = snprintf(pcWriteBuffer, xWriteBufferLen,
                          "Name             State    Prio  Stack  Time\r\n"
                          "-------------------------------------------\r\n");
    if (len >= xWriteBufferLen)
        return pdFALSE;

    char *writePtr = pcWriteBuffer + len;
    const char *line = rawBuffer;

    while (*line) {
        // Найдём конец строки
        const char *nextLine = strpbrk(line, "\r\n");

        size_t lineLen = nextLine ? (size_t) (nextLine - line) : strlen(line);
        if (lineLen == 0) {
            if (nextLine)
                line = nextLine + 1;
            else
                break;
            continue;
        }

        // Скопируем строку во временный буфер
        char tmp[128] = {0};
        strncpy(tmp, line, lineLen);
        tmp[lineLen] = '\0';

        // Распознать строку
        char name[20] = {0};
        char state[4] = {0};
        unsigned prio = 0, stack = 0, time = 0;

        if (sscanf(tmp, "%19s %3s %u %u %u", name, state, &prio, &stack, &time) >= 3) {
            const char *state_str = "?";
            switch (state[0]) {
                case 'R':
                    state_str = "Ready";
                    break;
                case 'B':
                    state_str = "Blocked";
                    break;
                case 'S':
                    state_str = "Suspended";
                    break;
                case 'D':
                    state_str = "Deleted";
                    break;
                case 'X':
                    state_str = "None";
                    break;
                default:
                    state_str = "Unknown";
                    break;
            }

            int written = snprintf(writePtr, xWriteBufferLen - (writePtr - pcWriteBuffer),
                                   "%-16s %-8s %-5u %-6u %-3u%%\r\n",
                                   name, state_str, prio, stack, time);

            if (written < 0 || (writePtr - pcWriteBuffer + written) >= xWriteBufferLen)
                break;

            writePtr += written;
        }

        // Переходим к следующей строке
        if (nextLine)
            line = nextLine + 1;
        else
            break;

        // Пропускаем лишние \r или \n
        while (*line == '\r' || *line == '\n') line++;
    }

    return pdFALSE;
}

static BaseType_t prvDfpCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    BaseType_t xParameterStringLength;
    const char *pcSubCmd = FreeRTOS_CLIGetParameter(pcCommandString,
                                                    1,
                                                    &xParameterStringLength);

    if (pcSubCmd == NULL) {
        snprintf(pcWriteBuffer,
                 xWriteBufferLen,
                 "Error: Missing parameters.\r\n");
        return pdFALSE;
    }

    // SubCommand play
    if (strncmp(pcSubCmd, "play", xParameterStringLength) == 0) {
        const char *trackNumStr;
        BaseType_t trackNumLen;
        int trackNum;

        trackNumStr = FreeRTOS_CLIGetParameter(pcCommandString, 2, &trackNumLen);
        if (trackNumStr == NULL) {
            snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: 'play' requires track number\r\n");
            return pdFALSE;
        }

        trackNum = atoi(trackNumStr);
        snprintf(pcWriteBuffer, xWriteBufferLen, "Playing track %d\r\n", trackNum);
    } else if (strncmp(pcSubCmd, "volume", xParameterStringLength) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Skipping to next track\r\n");
    } else if (strncmp(pcSubCmd, "next", xParameterStringLength) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Skipping to next track\r\n");
    } else if (strncmp(pcSubCmd, "prev", xParameterStringLength) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Return to previous track\r\n");
    } else if (strncmp(pcSubCmd, "pause", xParameterStringLength) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "The track has been stopped\r\n");
    } else if (strncmp(pcSubCmd, "resume", xParameterStringLength) == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "The track has been resumed\r\n");
    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Invalid subcommand\r\n");
    }

    return pdFALSE;
}

static BaseType_t prvFlashCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    return pdFALSE;
}

void UartCliThread(void *argument) {
    char input_buffer[128] = {0};
    char output_buffer[configCOMMAND_INT_MAX_OUTPUT_SIZE] = {0};
    uint8_t input_index = 0;
    uint8_t echo = 1;

    iprintf("\r\nFreeRTOS CLI v1.0\r\nType 'help' for commands\r\n\n> ");
    fflush(stdout);

    while (1) {
        uint8_t c;
        if (HAL_UART_Receive(&huart1, &c, 1, 100) == HAL_OK) {
            if (echo && isprint(c)) {
                iprintf("%c", c);
                fflush(stdout);

                // Явно передаем процессорное время другим задачам
                taskYIELD();  // Отдаем процессорное время Idle задаче
            }

            // Backspace или Delete
            if (c == '\b' || c == 0x7F) {
                if (input_index > 0) {
                    input_index--;
                    input_buffer[input_index] = '\0';
                    iprintf("\b \b");
                    fflush(stdout);
                }
                continue;
            }

            // Enter (CR или LF)
            if (c == '\r' || c == '\n') {
                iprintf("\r\n");

                if (input_index > 0) {
                    BaseType_t cmd_result;
                    do {
                        cmd_result = FreeRTOS_CLIProcessCommand(
                                input_buffer,
                                output_buffer,
                                sizeof(output_buffer));

                        if (strlen(output_buffer) > 0) {
                            iprintf("%s", output_buffer);
                        }
                        memset(output_buffer, 0, sizeof(output_buffer));
                    } while (cmd_result != pdFALSE);

                    memset(input_buffer, 0, sizeof(input_buffer));
                    input_index = 0;
                }

                iprintf("> ");
                fflush(stdout);

                // После завершения обработки команды, тоже можно отдать процессорное время
                taskYIELD();  // Отдаем процессорное время Idle задаче
                continue;
            }

            // Добавление печатных символов
            if (isprint(c) && input_index < sizeof(input_buffer) - 1) {
                input_buffer[input_index++] = c;
            }
        }
    }
}

void UART_CLI_Init(void) {
    if (FreeRTOS_CLIRegisterCommand(&xClearCommand) != pdPASS) {
        print_log(ERROR_LOG, "Failed to register xClearCommand\r\n");
    }

    if (FreeRTOS_CLIRegisterCommand(&xResetCommand) != pdPASS) {
        print_log(ERROR_LOG, "Failed to register xResetCommand\r\n");
    }

    if (FreeRTOS_CLIRegisterCommand(&xGpioCommand) != pdPASS) {
        print_log(ERROR_LOG, "Failed to register xLedCommand\r\n");
    }

    if (FreeRTOS_CLIRegisterCommand(&xTasksCommand) != pdPASS) {
        print_log(ERROR_LOG, "Failed to register xTasksCommand\r\n");
    }

    if (FreeRTOS_CLIRegisterCommand(&xDfpCommand) != pdPASS) {
        print_log(ERROR_LOG, "Failed to register xDfpCommand\r\n");
    }

    if (FreeRTOS_CLIRegisterCommand(&xFlashCommand) != pdPASS) {
        print_log(ERROR_LOG, "Failed to register xFlashCommand\r\n");
    }

    UartCliTaskHandle = xTaskCreateStatic(UartCliThread,
                                          "CLI-task",
                                          UART_CLI_TASK_STACK_SIZE,
                                          NULL,
                                          1,
                                          UartCliTaskStack,
                                          &UartCliTaskBuffer);

    if (UartCliTaskHandle == NULL) {
        print_log(ERROR_LOG, "Error creating UART CLI task\r\n");
    }
}
