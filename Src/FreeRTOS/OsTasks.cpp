#include "Main.hpp"

#define TASK1_STACK_SIZE 2048
#define TASK2_STACK_SIZE 2048

TaskHandle_t Task1Handle = nullptr;
TaskHandle_t Task2Handle = nullptr;

StaticTask_t Task1Buffer;
StackType_t Task1Stack[TASK1_STACK_SIZE];

StaticTask_t Task2Buffer;
StackType_t Task2Stack[TASK2_STACK_SIZE];

static StaticTimer_t blinkTimerBuffer;

extern QueueHandle_t uartQueue;
extern void QueuesInit();

extern SemaphoreHandle_t twiSemaphore;
extern void SemphrsInit();

extern FatFsWrapper uSD;

/**
 * @brief Поток для обработки UI (задача с периодом 5 мс)
 * @note LVGL не является потокобезопасной библиотекой!
 *       Все изменения, касающиеся UI, должны выполняться в этом потоке!
 * @param argument
 */
void Thread1(void *argument) {
    portTickType xLastWakeTime = xTaskGetTickCount();
    uSD.Init();
    while (true) {
        //lv_task_handler();
        //ui_tick();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
    }
}

/**
 * @brief Задача для вывода информационных сообщений в UART (использует очередь)
 * @param argument
 */
void Thread2(void *argument) {
    while (1) {
        uint8_t ch;
        if (xQueueReceive(uartQueue, &ch, portMAX_DELAY) == pdTRUE) {
            HAL_UART_Transmit(&huart1, &ch, 1, 100);
        }
    }
}

void BlinkCallback(TimerHandle_t xTimer) {
    BSP_LED_Toggle(LED_GREEN);
}

void FreeRTOS_Resources_Init() {
    UART_CLI_Init();
    QueuesInit();
    SemphrsInit();

    Task1Handle = xTaskCreateStatic(Thread1,
                                    "Task1",
                                    TASK1_STACK_SIZE,
                                    nullptr,
                                    5,
                                    Task1Stack,
                                    &Task1Buffer);

    if (Task1Handle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task1\r\n");
    }

    Task2Handle = xTaskCreateStatic(Thread2,
                                    "Task2",
                                    TASK2_STACK_SIZE,
                                    nullptr,
                                    3,
                                    Task2Stack,
                                    &Task2Buffer);

    if (Task2Handle == nullptr) {
        print_log(ERROR_LOG, "Error creating Task2\r\n");
    }

    TimerHandle_t blinkTimer = xTimerCreateStatic(
            "Blink",
            pdMS_TO_TICKS(1000),
            pdTRUE,
            nullptr,
            BlinkCallback,
            &blinkTimerBuffer
    );

    // Запуск таймера
    if (blinkTimer != nullptr) {
        xTimerStart(blinkTimer, 0);
    }
}
