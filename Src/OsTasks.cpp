#include "OsTasks.h"

#define WATCHDOG_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define LVGL_TASK_STACK_SIZE 2048
#define UART_TASK_STACK_SIZE 256
#define TWI_TASK_STACK_SIZE 512
#define STACK4_SIZE 256

#define UART_QUEUE_LENGTH 512
#define UART_QUEUE_ITEM_SIZE sizeof(char)

extern FatFsWrapper uSD;

extern QSPI_HandleTypeDef QSPIHandle;
QSPI extFlash(&QSPIHandle);
DS18X20 ds18s20(GPIOA, GPIO_PIN_0, &htim5);

TaskHandle_t WatchdogTaskHandle = nullptr;
TaskHandle_t LvglTaskHandle = nullptr;
TaskHandle_t UartTaskHandle = nullptr;
TaskHandle_t TwiTaskHandle = nullptr;
TaskHandle_t Task4Handle = nullptr;

SemaphoreHandle_t twiSemaphore = nullptr;
static StaticSemaphore_t twiSemaphoreBuffer;

QueueHandle_t uartQueue = nullptr;
static StaticQueue_t xUartQueueStruct;
static uint8_t ucUartQueueStorageArea[UART_QUEUE_LENGTH * UART_QUEUE_ITEM_SIZE];

StaticTask_t WatchdogTaskBuffer;
StackType_t WatchdogTaskStack[WATCHDOG_TASK_STACK_SIZE];

StaticTask_t LvglTaskBuffer;
StackType_t LvglTaskStack[LVGL_TASK_STACK_SIZE];

StaticTask_t UartTaskBuffer;
StackType_t UartTaskStack[UART_TASK_STACK_SIZE];

StaticTask_t TwiTaskBuffer;
StackType_t TwiTaskStack[TWI_TASK_STACK_SIZE];

StaticTask_t Task4Buffer;
StackType_t Stack4[STACK4_SIZE];

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

#define MAX_MISSED_CYCLES 2
#define WDG_CHECK_INTERVAL_MS 500

volatile uint8_t LvglTaskAlive = 0;
extern volatile uint8_t UartCliAlive;

/**
 * @brief Поток для перезагрузки Watchdog таймера
 * @param argument
 */
void WatchdogThread(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();
    MX_IWDG_Init();
    for (;;) {
        BaseType_t all_tasks_alive = 1;

        if (!LvglTaskAlive || !UartCliAlive) {
            all_tasks_alive = 0;
        }

        if (all_tasks_alive) {
            HAL_IWDG_Refresh(&hiwdg);
        } else {
            // Индикация ошибки
        }

        LvglTaskAlive = 0;
        UartCliAlive = 0;

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Поток для обработки UI (задача с периодом 5 мс)
 * @note LVGL не является потокобезопасной библиотекой!
 *       Все изменения, касающиеся UI, должны выполняться в этом потоке!
 * @param argument
 */
void LvglThread(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();
    //uSD.Init();
    //extFlash.Init();
    //lv_fs_fatfs_init();
    while (1) {
        lv_task_handler();
        ui_tick();
        LvglTaskAlive = 1;
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
    }
}

/**
 * @brief Задача для вывода информационных сообщений в UART (использует очередь)
 * @param argument
 */
void UartThread(void *argument) {
    while (1) {
        uint8_t ch;
        if (xQueueReceive(uartQueue, &ch, portMAX_DELAY) == pdTRUE) {
            HAL_UART_Transmit(&huart1, &ch, 1, 100);
        }
    }
}

void TwiThread(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}

void Task4Thread(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();

    ds18s20.init();
    ds18s20.handleEvent(OW::Event::Start);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2000));
    ds18s20.writeByte(SKIP_ROM);

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2000));
    }
}

void FreeRTOS_Resources_Init() {
    //UART_CLI_Init();

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
    /*
    WatchdogTaskHandle = xTaskCreateStatic(WatchdogThread,
                                           "Task for Watchdog Timer",
                                           WATCHDOG_TASK_STACK_SIZE,
                                           nullptr,
                                           4,
                                           WatchdogTaskStack,
                                           &WatchdogTaskBuffer);

    if (WatchdogTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating WatchdogTask\r\n");
    }
    */
    LvglTaskHandle = xTaskCreateStatic(LvglThread,
                                       "Task for LVGL interface",
                                       LVGL_TASK_STACK_SIZE,
                                       nullptr,
                                       5,
                                       LvglTaskStack,
                                       &LvglTaskBuffer);

    if (LvglTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating LvglTask\r\n");
    }

    UartTaskHandle = xTaskCreateStatic(UartThread,
                                       "Task for UART",
                                       UART_TASK_STACK_SIZE,
                                       nullptr,
                                       3,
                                       UartTaskStack,
                                       &UartTaskBuffer);

    if (UartTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating UartTask\r\n");
    }

    TwiTaskHandle = xTaskCreateStatic(TwiThread,
                                      "Task for TWI communication",
                                      TWI_TASK_STACK_SIZE,
                                      nullptr,
                                      2,
                                      TwiTaskStack,
                                      &TwiTaskBuffer);

    if (TwiTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating TwiTask\r\n");
    }

    Task4Handle = xTaskCreateStatic(Task4Thread,
                                    "Task4",
                                    STACK4_SIZE,
                                    nullptr,
                                    5,
                                    Stack4,
                                    &Task4Buffer);

    if (Task4Handle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task4\r\n");
    }
}

extern "C" int __io_putchar(int ch) {
    if (uartQueue == NULL) return EOF;

    auto c = (uint8_t) ch;
    BaseType_t status;

    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        status = xQueueSendFromISR(uartQueue, &c, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        status = xQueueSend(uartQueue, &c, pdMS_TO_TICKS(10));
    }

    return (status == pdTRUE) ? ch : EOF;
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == ds18s20.getTimerHandle()) {
        ds18s20.handleEvent(OW::Event::Timeout);
    }
    if (htim->Instance == TIM6) {
        HAL_IncTick();
        lv_tick_inc(1);
    }
}
