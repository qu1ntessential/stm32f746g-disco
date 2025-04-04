#include "OsTasks.h"

#define LVGL_TASK_STACK_SIZE 1024
#define UART_TASK_STACK_SIZE 256
#define STACK3_SIZE 1024
#define STACK4_SIZE 1024

#define UIEVENT_QUEUE_LENGTH    10
#define UIEVENT_QUEUE_ITEM_SIZE sizeof(UIEvent_t)

#define UICMD_QUEUE_LENGTH    10
#define UICMD_QUEUE_ITEM_SIZE sizeof(UICmd_t)

#define UART_QUEUE_LENGTH    400
#define UART_QUEUE_ITEM_SIZE sizeof(char)
#define TX_BUF_SIZE 200

#define PRE_RTOS_BUF_SIZE 500
static char pre_rtos_buf[PRE_RTOS_BUF_SIZE];
static size_t pre_rtos_index = 0;
static bool rtos_started = false;

TaskHandle_t LvglTaskHandle = nullptr;
TaskHandle_t UartTaskHandle = nullptr;
TaskHandle_t Task3Handle = nullptr;
TaskHandle_t Task4Handle = nullptr;

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

extern "C" void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                              StackType_t **ppxIdleTaskStackBuffer,
                                              uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("Stack overflow in task: %s\n", pcTaskName);

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
        //lv_task_handler();
        //ui_tick();
        vTaskDelayUntil(&xLastWakeTime, 5);
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

void FreeRTOS_Resources_Init() {
    LvglTaskHandle = xTaskCreateStatic(LvglThread,
                                       "Task for LVGL interface",
                                       LVGL_TASK_STACK_SIZE,
                                       nullptr,
                                       5,
                                       LvglTaskStack,
                                       &LvglTaskBuffer);

    if (LvglTaskHandle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task1\r\n");

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
    }
}
