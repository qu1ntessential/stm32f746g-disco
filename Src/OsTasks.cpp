#include "OsTasks.h"

#define LVGL_TASK_STACK_SIZE 1024
#define UART_TASK_STACK_SIZE 256
#define TWI_TASK_STACK_SIZE 512
#define STACK4_SIZE 256
#define CLI_TASK_STACK_SIZE 256

#define UART_QUEUE_LENGTH 400
#define UART_QUEUE_ITEM_SIZE sizeof(char)
#define TX_BUF_SIZE 200

#define PRE_RTOS_BUF_SIZE 500
static char pre_rtos_buf[PRE_RTOS_BUF_SIZE];
static size_t pre_rtos_index = 0;
static bool rtos_started = false;

extern FatFsWrapper uSD;
extern ESG ESG15;

TaskHandle_t LvglTaskHandle = nullptr;
TaskHandle_t UartTaskHandle = nullptr;
TaskHandle_t TwiTaskHandle = nullptr;
TaskHandle_t Task4Handle = nullptr;
TaskHandle_t cliTaskHandle = nullptr;

SemaphoreHandle_t twiSemaphore = nullptr;
static StaticSemaphore_t twiSemaphoreBuffer;

QueueHandle_t uartQueue = nullptr;
static StaticQueue_t xUartQueueStruct;
static uint8_t ucUartQueueStorageArea[UART_QUEUE_LENGTH * UART_QUEUE_ITEM_SIZE];

StaticTask_t LvglTaskBuffer;
StackType_t LvglTaskStack[LVGL_TASK_STACK_SIZE];

StaticTask_t UartTaskBuffer;
StackType_t UartTaskStack[UART_TASK_STACK_SIZE];

StaticTask_t TwiTaskBuffer;
StackType_t TwiTaskStack[TWI_TASK_STACK_SIZE];

StaticTask_t Task4Buffer;
StackType_t Stack4[STACK4_SIZE];

StaticTask_t cliTaskBuffer;
StackType_t cliTaskStack[CLI_TASK_STACK_SIZE];

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

/**
 * @brief Функция, предоставляющая FreeRTOS память для статического создания задачи IDLE
 * @param xIdleTaskTCBBuffer Указатель на буфер для TCB (Task Control Block)
 * @param uxIdleTaskStack Указатель на стек задачи IDLE
 * @param IdleTaskStackSize Размер стека
 */
extern "C" void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                              StackType_t **ppxIdleTaskStackBuffer,
                                              uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * @brief Функция-обработчик, вызываемая FreeRTOS при обнаружении переполнения стека в любой задаче
 * @param xTask Handle задачи, где произошло переполнение
 * @param pcTaskName Имя задачи, у которой переполнился стек
 */
extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    print_log(ERROR_LOG, "Stack overflow in task: %s\n", pcTaskName);
    while (1) {}
}

/*
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    /// Статический буфер для TCB (Task Control Block)
    static StaticTask_t xTimerTaskTCB;

    /// Статический стек для задачи таймеров
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
*/

/**
 * @brief Поток для обработки UI (задача с периодом 5 мс)
 * @note LVGL не является потокобезопасной библиотекой!
 *       Все изменения, касающиеся UI, должны выполняться в этом потоке!
 * @param argument
 */
void LvglThread(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (1) {
        ui_tick();
        lv_task_handler();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
    }
}

/**
 * @brief Задача для вывода информационных сообщений в UART (использует очередь)
 * @param argument
 */
void UartThread(void *argument) {
    rtos_started = true;

    // Отправляем буфер с ранними сообщениями
    for (size_t i = 0; i < pre_rtos_index; i++) {
        xQueueSend(uartQueue, &pre_rtos_buf[i], portMAX_DELAY);
    }
    pre_rtos_index = 0;

    uint8_t buf[TX_BUF_SIZE];
    uint8_t index = 0;

    while (1) {
        uint8_t ch;
        if (xQueueReceive(uartQueue, &ch, portMAX_DELAY) == pdTRUE) {
            buf[index++] = ch;

            /// Отправляем буфер, если он заполнен или получен символ новой строки
            if (index >= TX_BUF_SIZE || ch == '\n') {
                HAL_UART_Transmit(&huart1, buf, index, HAL_MAX_DELAY);
                index = 0;
            }
        }
    }
}

void TwiThread(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();
    ESG15.setTimeout();
    ESG15.getStateTwi();
    if (ESG15.getMonoBiFlag()) {
        ESG15.setMonoCutPower();
        ESG15.setMonoMixPower();
    } else {
        ESG15.setBiCutPower();
        ESG15.setBiMixPower();
    }
    ESG15.setMonoCoagPower();
    ESG15.setBiCoagPower();
    ESG15.syncUI();
    while (1) {
        ESG15.getStateTwi();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}

void Task4Thread(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (true) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}

void cliThread(void *argument) {
    char input_buffer[128] = {0};  // Буфер ввода
    char output_buffer[configCOMMAND_INT_MAX_OUTPUT_SIZE] = {0}; // Буфер вывода
    uint8_t input_index = 0;
    uint8_t echo = 1; // Флаг эхо-вывода

    printf("FreeRTOS CLI v1.0\r\nType 'help' for commands\r\n> ");

    while (1) {
        uint8_t c;
        if (HAL_UART_Receive(&huart1, &c, 1, portMAX_DELAY) == HAL_OK) {
            // Эхо-вывод только печатных символов
            if (echo && isprint(c)) {
                HAL_UART_Transmit(&huart1, &c, 1, 10);
            }

            // Обработка Backspace
            if (c == '\b') {
                if (input_index > 0) {
                    input_buffer[--input_index] = '\0';
                    // Отправляем backspace + space + backspace для очистки символа
                    uint8_t bs_seq[] = {'\b', ' ', '\b'};
                    HAL_UART_Transmit(&huart1, bs_seq, 3, 10);
                }
                continue;
            }

            // Обработка Enter
            if (c == '\r' || c == '\n') {
                if (c == '\r') {
                    HAL_UART_Transmit(&huart1, (uint8_t *) "\r\n", 2, 10);
                }

                if (input_index > 0) {
                    // Обработка команды
                    BaseType_t cmd_result;
                    do {
                        cmd_result = FreeRTOS_CLIProcessCommand(
                                input_buffer,
                                output_buffer,
                                sizeof(output_buffer));

                        if (strlen(output_buffer) > 0) {
                            HAL_UART_Transmit(&huart1, (uint8_t *) output_buffer, strlen(output_buffer), 10);
                        }
                        memset(output_buffer, 0, sizeof(output_buffer));
                    } while (cmd_result != pdFALSE);

                    // Очистка буфера ввода
                    memset(input_buffer, 0, sizeof(input_buffer));
                    input_index = 0;
                }

                // Вывод приглашения
                HAL_UART_Transmit(&huart1, (uint8_t *) "> ", 2, 10);
                continue;
            }

            // Добавление печатных символов в буфер
            if (isprint(c) && input_index < sizeof(input_buffer) - 1) {
                input_buffer[input_index++] = c;
            }
        }
    }
}

void FreeRTOS_Resources_Init() {
    if (FreeRTOS_CLIRegisterCommand(&xLedCommand) != pdPASS) {
        printf("Failed to register xLedCommand\r\n");
    }

    if (FreeRTOS_CLIRegisterCommand(&xTasksCommand) != pdPASS) {
        printf("Failed to register xTasksCommand\r\n");
    }

    twiSemaphore = xSemaphoreCreateBinaryStatic(&twiSemaphoreBuffer);

    if (twiSemaphore == nullptr) {
        print_log(ERROR_LOG, "Error creating twiSemaphore\n\r");
    }

    uartQueue = xQueueCreateStatic(UART_QUEUE_LENGTH,
                                   UART_QUEUE_ITEM_SIZE,
                                   ucUartQueueStorageArea,
                                   &xUartQueueStruct);

    if (uartQueue == nullptr) {
        print_log(ERROR_LOG, "Error creating uiCmdQueue\n\r");
    } else {
        vQueueAddToRegistry(uartQueue, "uartQueue");
    }

    LvglTaskHandle = xTaskCreateStatic(LvglThread,
                                       "Task for LVGL interface",
                                       LVGL_TASK_STACK_SIZE,
                                       nullptr,
                                       3,
                                       LvglTaskStack,
                                       &LvglTaskBuffer);

    if (LvglTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task1\r\n");
    }

    UartTaskHandle = xTaskCreateStatic(UartThread,
                                       "Task for UART",
                                       UART_TASK_STACK_SIZE,
                                       nullptr,
                                       3,
                                       UartTaskStack,
                                       &UartTaskBuffer);

    if (UartTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task2\r\n");
    }

    TwiTaskHandle = xTaskCreateStatic(TwiThread,
                                      "Task for TWI communication",
                                      TWI_TASK_STACK_SIZE,
                                      nullptr,
                                      1,
                                      TwiTaskStack,
                                      &TwiTaskBuffer);

    if (TwiTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task3\r\n");
    }

    Task4Handle = xTaskCreateStatic(Task4Thread,
                                    "Task4",
                                    STACK4_SIZE,
                                    nullptr,
                                    1,
                                    Stack4,
                                    &Task4Buffer);

    if (Task4Handle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task4\r\n");
    }

    cliTaskHandle = xTaskCreateStatic(cliThread,
                                      "CLI task thread",
                                      CLI_TASK_STACK_SIZE,
                                      nullptr,
                                      2,
                                      cliTaskStack,
                                      &cliTaskBuffer);

    if (cliTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task5\r\n");
    }
}

extern "C" int __io_putchar(int ch) {
    if (!rtos_started) {
        if (pre_rtos_index < PRE_RTOS_BUF_SIZE) {
            pre_rtos_buf[pre_rtos_index++] = ch;
        }
        return ch;
    }

    if (uartQueue == nullptr) {
        return EOF;
    }

    auto c = (uint8_t) ch;
    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(uartQueue, &c, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        if (xQueueSend(uartQueue, &c, 10 / portTICK_PERIOD_MS) != pdTRUE) {
            return EOF;
        }
    }
    return ch;
}
