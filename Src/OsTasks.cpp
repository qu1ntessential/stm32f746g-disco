#include "OsTasks.h"

#define LVGL_TASK_STACK_SIZE 1024
#define UART_TASK_STACK_SIZE 256
#define STACK3_SIZE 1024
#define STACK4_SIZE 1024

#define UIEVENT_QUEUE_LENGTH 10                   ///< Длина очереди UIEvent
#define UIEVENT_QUEUE_ITEM_SIZE sizeof(UIEvent_t) ///< Размер элемента очереди UIEvent

#define UICMD_QUEUE_LENGTH 10
#define UICMD_QUEUE_ITEM_SIZE sizeof(UICmd_t)

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
TaskHandle_t Task3Handle = nullptr;
TaskHandle_t Task4Handle = nullptr;

SemaphoreHandle_t twiSemaphore = nullptr;
static StaticSemaphore_t twiSemaphoreBuffer;

QueueHandle_t uiEventQueue = nullptr;
static StaticQueue_t xUiEventQueueStruct;
static uint8_t ucUiEventQueueStorageArea[UIEVENT_QUEUE_LENGTH * UIEVENT_QUEUE_ITEM_SIZE];

QueueHandle_t uiCmdQueue = nullptr;
static StaticQueue_t xUiCmdQueueStruct;
static uint8_t ucUiCmdQueueStorageArea[UICMD_QUEUE_LENGTH * UICMD_QUEUE_ITEM_SIZE];

QueueHandle_t uartQueue = nullptr;
static StaticQueue_t xUartQueueStruct;
static uint8_t ucUartQueueStorageArea[UART_QUEUE_LENGTH * UART_QUEUE_ITEM_SIZE];

StaticTask_t LvglTaskBuffer;
StackType_t LvglTaskStack[LVGL_TASK_STACK_SIZE];

StaticTask_t UartTaskBuffer;
StackType_t UartTaskStack[UART_TASK_STACK_SIZE];

StaticTask_t Task3Buffer;
StackType_t Stack3[STACK3_SIZE];

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

/**
 * @brief Поток для обработки UI
 * @note LVGL не является потокобезопасной библиотекой!
 *       Все изменения, касающиеся UI, должны выполняться в этом потоке!
 * @param argument
 */
void LvglThread(void *argument) {
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        lv_task_handler();
        ui_tick();
        vTaskDelayUntil(&xLastWakeTime, 5);
    }
}

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

void Task3Thread(void *argument) {
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(300));
    }
}

void Task4Thread(void *argument) {
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while (true) {
        ESG15.setBiCoagPower(300, 0);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10000));
    }
}

void FreeRTOS_Resources_Init() {
    twiSemaphore = xSemaphoreCreateBinaryStatic(&twiSemaphoreBuffer);

    if (twiSemaphore == nullptr) {
        print_log(ERROR_LOG, "Error creating twiSemaphore\n\r");
    }

    uiEventQueue = xQueueCreateStatic(UIEVENT_QUEUE_LENGTH,
                                      UIEVENT_QUEUE_ITEM_SIZE,
                                      ucUiEventQueueStorageArea,
                                      &xUiEventQueueStruct);

    if (uiEventQueue == nullptr) {
        print_log(ERROR_LOG, "Error creating uiEventQueue\n\r");
    }

    uiCmdQueue = xQueueCreateStatic(UICMD_QUEUE_LENGTH,
                                    UICMD_QUEUE_ITEM_SIZE,
                                    ucUiCmdQueueStorageArea,
                                    &xUiCmdQueueStruct);

    if (uiCmdQueue == nullptr) {
        print_log(ERROR_LOG, "Error creating uiCmdQueue\n\r");
    }

    uartQueue = xQueueCreateStatic(UART_QUEUE_LENGTH,
                                   UART_QUEUE_ITEM_SIZE,
                                   ucUartQueueStorageArea,
                                   &xUartQueueStruct);

    if (uartQueue == nullptr) {
        print_log(ERROR_LOG, "Error creating uiCmdQueue\n\r");
    }

    LvglTaskHandle = xTaskCreateStatic(LvglThread,
                                       "Task for LVGL interface",
                                       LVGL_TASK_STACK_SIZE,
                                       nullptr,
                                       5,
                                       LvglTaskStack,
                                       &LvglTaskBuffer);

    if (LvglTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task1\r\n");
    }

    UartTaskHandle = xTaskCreateStatic(UartThread,
                                       "Task for UART",
                                       UART_TASK_STACK_SIZE,
                                       nullptr,
                                       2,
                                       UartTaskStack,
                                       &UartTaskBuffer);

    if (UartTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task2\r\n");
    }

    Task3Handle = xTaskCreateStatic(Task3Thread,
                                    "Task3",
                                    STACK3_SIZE,
                                    nullptr,
                                    5,
                                    Stack3,
                                    &Task3Buffer);

    if (Task3Handle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task3\r\n");
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
