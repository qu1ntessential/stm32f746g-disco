#include "OsTasks.h"

#define LVGL_TASK_STACK_SIZE 1024
#define STACK3_SIZE 1024

TaskHandle_t LvglTaskHandle = nullptr;
TaskHandle_t Task3Handle = nullptr;

StaticTask_t LvglTaskBuffer;
StackType_t LvglTaskStack[LVGL_TASK_STACK_SIZE];

StaticTask_t Task3Buffer;
StackType_t Stack3[STACK3_SIZE];

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
