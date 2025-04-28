#include "Commands.h"

#define LED_GPIO_Port GPIOI
#define LED_Pin       GPIO_PIN_1

static BaseType_t prvLEDCommand(char *pcWriteBuffer,
                                size_t xWriteBufferLen,
                                const char *pcCommandString);

static BaseType_t prvTasksCommand(char *pcWriteBuffer,
                                  size_t xWriteBufferLen,
                                  const char *pcCommandString);

const CLI_Command_Definition_t xLedCommand = {
        "led",                           // Имя команды (должно быть в нижнем регистре)
        "led [on|off]: Control LED\r\n", // Справка
        prvLEDCommand,            // Функция-обработчик
        1                   // Ожидаемое количество параметров
};

const CLI_Command_Definition_t xTasksCommand = {
        "tasks", // Имя команды
        "tasks: List all running tasks\r\n"
        "Format: <Name> <State> <Priority> <Runtime>\r\n"
        "States: R-Running, Y-Ready, B-Blocked, S-Suspended, D-Deleted\r\n",
        prvTasksCommand,
        0 // Без параметров
};

// Пример команды для управления светодиодом
static BaseType_t prvLEDCommand(char *pcWriteBuffer,
                                size_t xWriteBufferLen,
                                const char *pcCommandString) {
    // Извлекаем параметры
    BaseType_t paramLen;
    const char *param = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    // Проверяем параметр
    if (param == NULL) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: Specify 'on' or 'off'\r\n");
        return pdFALSE;
    }

    // Обработка команд
    if (strncmp(param, "on", paramLen) == 0) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        snprintf(pcWriteBuffer, xWriteBufferLen, "LED turned ON\r\n");
    } else if (strncmp(param, "off", paramLen) == 0) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        snprintf(pcWriteBuffer, xWriteBufferLen, "LED turned OFF\r\n");
    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: Invalid parameter\r\n");
    }

    return pdFALSE; // Команда завершена
}

static BaseType_t prvTasksCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    BaseType_t paramLen;
    const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    if (param1 != NULL) {
        // Есть первый параметр (suspend/resume)
        if (strncmp(param1, "suspend", paramLen) == 0 || strncmp(param1, "resume", paramLen) == 0) {
            const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 2, &paramLen);
            if (taskName == NULL) {
                snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: Specify task name\r\n");
                return pdFALSE;
            }

            // Нужно скопировать имя задачи, потому что taskName не null-terminated
            char taskNameBuffer[configMAX_TASK_NAME_LEN + 1] = {0};
            strncpy(taskNameBuffer, taskName, paramLen);
            taskNameBuffer[paramLen] = '\0';

            // Ищем задачу
            TaskHandle_t taskHandle = xTaskGetHandle(taskNameBuffer);
            if (taskHandle == NULL) {
                snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: Task '%s' not found\r\n", taskNameBuffer);
                return pdFALSE;
            }

            if (strncmp(param1, "suspend", paramLen) == 0) {
                vTaskSuspend(taskHandle);
                snprintf(pcWriteBuffer, xWriteBufferLen, "Task '%s' suspended\r\n", taskNameBuffer);
            } else {
                vTaskResume(taskHandle);
                snprintf(pcWriteBuffer, xWriteBufferLen, "Task '%s' resumed\r\n", taskNameBuffer);
            }

            return pdFALSE;
        } else {
            snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR: Invalid command. Use 'suspend' or 'resume'\r\n");
            return pdFALSE;
        }
    }

    // Если параметров нет — печатаем список задач
    // Убедись, что включено: configUSE_TRACE_FACILITY = 1 и configUSE_STATS_FORMATTING_FUNCTIONS = 1

    vTaskList(pcWriteBuffer); // Выводим список задач в буфер
    return pdFALSE; // Вывод завершен
}
