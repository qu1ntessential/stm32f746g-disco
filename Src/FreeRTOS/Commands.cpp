#include "Commands.hpp"

#include <stdlib.h>

TaskHandle_t UartCliTaskHandle = NULL;
static StaticTask_t UartCliTaskBuffer;
static StackType_t UartCliTaskStack[UART_CLI_TASK_STACK_SIZE];

volatile uint8_t UartCliAlive = 0;

extern FatFsWrapper uSD;

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

static BaseType_t prvFileSystemCommand(char *pcWriteBuffer,
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

static const CLI_Command_Definition_t xFileSystemCommand = {
        "fs", // Имя команды
        "fs:\r\n\tFilesystem control commands:\r\n"
        "\tfs ls [path]\r\n\n",
        prvFileSystemCommand,
        -1
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

    // Сброс через системный контроллер
    NVIC_SystemReset();

    return pdFALSE;
}

static BaseType_t prvGpioCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    BaseType_t xParameterStringLength = 0;
    const char *pcPortParam = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);
    
    if (!pcPortParam) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Missing port parameter.\r\n");
        return pdFALSE;
    }

    // Port validation
    char portChar = pcPortParam[0];
    GPIO_TypeDef *GPIOx = get_gpio_port(portChar);
    if (!GPIOx) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Invalid GPIO port '%c'. Use A-I.\r\n", portChar);
        return pdFALSE;
    }

    // Pin parameter
    xParameterStringLength = 0;
    const char *pcPinParam = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength);
    if (!pcPinParam || xParameterStringLength == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Missing pin parameter.\r\n");
        return pdFALSE;
    }
    
    char pinStr[4] = {0};
    size_t pinLen = xParameterStringLength < 3 ? xParameterStringLength : 3;
    strncpy(pinStr, pcPinParam, pinLen);
    pinStr[pinLen] = '\0';
    
    char *endptr = NULL;
    long pin = strtol(pinStr, &endptr, 10);
    if (*endptr != '\0' || pin < 0 || pin > 15) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Invalid GPIO pin. Use 0-15.\r\n");
        return pdFALSE;
    }

    uint32_t pinMask = (1U << pin);

    // State parameter
    xParameterStringLength = 0;
    const char *pcStateParam = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParameterStringLength);
    if (!pcStateParam || xParameterStringLength == 0) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Missing action parameter (set/reset/read).\r\n");
        return pdFALSE;
    }
    
    char action[8] = {0};
    size_t actionLen = xParameterStringLength < 7 ? xParameterStringLength : 7;
    strncpy(action, pcStateParam, actionLen);
    action[actionLen] = '\0';

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
    // Локальные переменные вместо static для экономии RAM
    static bool tasksStarted = false;
    static const char *currentLine = NULL;
    char *rawBuffer = NULL;

    // Первый вызов: инициализация
    if (!tasksStarted) {
        // Выделяем буфер динамически вместо static
        rawBuffer = (char *)pvPortMalloc(2048);
        if (rawBuffer == NULL) {
            snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Not enough memory for task list\r\n");
            return pdFALSE;
        }
        
        vTaskList(rawBuffer);
        currentLine = rawBuffer;
        tasksStarted = true;
        
        // Заголовок
        size_t len = snprintf(pcWriteBuffer, xWriteBufferLen,
                              "Name             State    Prio  Stack  Time\r\n"
                              "-------------------------------------------\r\n");
        if (len >= xWriteBufferLen)
            return pdFALSE;
        return pdTRUE;
    }

    // Пошаговый вывод строк
    const char *line = currentLine;
    size_t bytesWritten = 0;

    while (*line && bytesWritten < xWriteBufferLen) {
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

        // Безопасное копирование строки
        char tmp[96] = {0};
        size_t copyLen = lineLen < 95 ? lineLen : 95;
        strncpy(tmp, line, copyLen);
        tmp[copyLen] = '\0';

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

            int written = snprintf(pcWriteBuffer + bytesWritten,
                                   xWriteBufferLen - bytesWritten,
                                   "%-16s %-8s %-5u %-6u %-3u%%\r\n",
                                   name, state_str, prio, stack, time);

            if (written < 0 || (size_t)written >= (xWriteBufferLen - bytesWritten))
                break;

            bytesWritten += written;
        }

        // Переходим к следующей строке
        if (nextLine)
            line = nextLine + 1;
        else
            break;

        // Пропускаем лишние \r или \n
        while (*line == '\r' || *line == '\n') line++;
    }

    currentLine = line;

    if (*line == '\0') {
        // Закончили вывод - освобождаем память
        tasksStarted = false;
        if (rawBuffer != NULL) {
            vPortFree(rawBuffer);
            rawBuffer = NULL;
        }
        return pdFALSE;
    }

    return pdTRUE;  // Продолжить вызовы
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
        BaseType_t trackNumLen = 0;
        int trackNum;

        trackNumStr = FreeRTOS_CLIGetParameter(pcCommandString, 2, &trackNumLen);
        if (trackNumStr == NULL) {
            snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: 'play' requires track number\r\n");
            return pdFALSE;
        }

        trackNum = atoi(trackNumStr);
        snprintf(pcWriteBuffer, xWriteBufferLen, "Playing track %d\r\n", trackNum);
    } else if (strncmp(pcSubCmd, "volume", xParameterStringLength) == 0) {
        const char *volStr = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameterStringLength);
        if (volStr == NULL) {
            snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: 'volume' requires level (0-30)\r\n");
            return pdFALSE;
        }
        int vol = atoi(volStr);
        snprintf(pcWriteBuffer, xWriteBufferLen, "Volume set to %d\r\n", vol);
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

static BaseType_t prvFileSystemCommand(char *pcWriteBuffer,
                                       size_t xWriteBufferLen,
                                       const char *pcCommandString) {
    static std::vector<std::string> files;
    static size_t currentIndex = 0;
    static bool listingActive = false;
    static StaticSemaphore_t xMutexBuffer;
    static SemaphoreHandle_t xMutex = NULL;
    
    // Создаём мьютекс в первый раз
    if (xMutex == NULL) {
        xMutex = xSemaphoreCreateMutexStatic(&xMutexBuffer);
    }

    BaseType_t xParameterStringLength;
    const char *pcSubCmd = FreeRTOS_CLIGetParameter(pcCommandString,
                                                    1,
                                                    &xParameterStringLength);

    if (xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Error: Mutex timeout\r\n");
        return pdFALSE;
    }

    // Первый вызов: инициализация
    if (!listingActive) {
        if (pcSubCmd == NULL) {
            xSemaphoreGive(xMutex);
            snprintf(pcWriteBuffer, xWriteBufferLen,
                     "Error: Missing parameters.\r\n");
            return pdFALSE;
        }

        if (strncmp(pcSubCmd, "ls", xParameterStringLength) == 0) {
            const char *path;
            BaseType_t pathLen;

            path = FreeRTOS_CLIGetParameter(pcCommandString, 2, &pathLen);
            if (path == NULL) {
                snprintf(pcWriteBuffer, xWriteBufferLen,
                         "ERROR: 'ls' requires path\r\n");
                return pdFALSE;
            }

            // Получаем путь как std::string
            std::string pathStr(path, pathLen);
            files.clear();
            currentIndex = 0;

            if (uSD.ls(pathStr.c_str(), files) != FatFsWrapper::Result::OK) {
                xSemaphoreGive(xMutex);
                snprintf(pcWriteBuffer, xWriteBufferLen,
                         "ERROR: ls failed on path '%s'\r\n", pathStr.c_str());
                return pdFALSE;
            }

            if (files.empty()) {
                xSemaphoreGive(xMutex);
                snprintf(pcWriteBuffer, xWriteBufferLen,
                         "Directory is empty.\r\n");
                return pdFALSE;
            }

            listingActive = true;
        }
    }

    // Пошаговая генерация вывода
    size_t bytesWritten = 0;
    for (; currentIndex < files.size(); currentIndex++) {
        const std::string &entry = files[currentIndex];
        int n = snprintf(pcWriteBuffer + bytesWritten,
                         xWriteBufferLen - bytesWritten,
                         "%s\r\n", entry.c_str());

        if (n < 0 || n >= (int) (xWriteBufferLen - bytesWritten)) {
            break;  // Буфер переполнен
        }

        bytesWritten += n;
    }

    if (currentIndex >= files.size()) {
        // Закончили вывод
        listingActive = false;
        xSemaphoreGive(xMutex);
        return pdFALSE;
    }

    xSemaphoreGive(xMutex);
    return pdTRUE; // Продолжить вызовы
}

void UartCliThread(void *argument) {
    char input_buffer[128] = {0};
    char output_buffer[configCOMMAND_INT_MAX_OUTPUT_SIZE] = {0};
    size_t input_index = 0;
    uint8_t echo = 1;

    iprintf("\r\nFreeRTOS CLI v1.0\r\nType 'help' for commands\r\n\n> ");
    fflush(stdout);

    while (1) {
        uint8_t c;
        if (HAL_UART_Receive(&huart1, &c, 1, 100) == HAL_OK) {
            // Отправляем полученный символ обратно (echo)
            if (echo && isprint(c)) {
                iprintf("%c", c);
                fflush(stdout);
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
                continue;
            }

            // Добавление печатных символов с проверкой переполнения буфера
            if (isprint(c) && input_index < sizeof(input_buffer) - 1) {
                input_buffer[input_index++] = c;
            } else if (!isprint(c)) {
                // Игнорируем управляющие символы
                continue;
            } else if (input_index >= sizeof(input_buffer) - 1) {
                // Буфер полон
                iprintf("\r\n[Buffer full]\r\n> ");
                fflush(stdout);
                memset(input_buffer, 0, sizeof(input_buffer));
                input_index = 0;
            }
        }
        UartCliAlive = 1;
        
        // Отдаём процессорное время один раз за итерацию цикла
        taskYIELD();
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

    if (FreeRTOS_CLIRegisterCommand(&xFileSystemCommand) != pdPASS) {
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
