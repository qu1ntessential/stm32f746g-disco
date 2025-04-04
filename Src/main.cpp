#include "OsTasks.h"

#include "lvgl_port_tft.h"
#include "lvgl_port_touchpad.h"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);
extern "C" int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, HAL_MAX_DELAY);
    return ch;
}

int main() {
    HAL_Init();

    SystemClock_Config();
    PeriphCommonClock_Config();

    SCB_EnableICache();
    SCB_EnableDCache();

    lv_init();
    tft_init();
    touchpad_init();
    ui_init();

    BSP_LED_Init(LED_GREEN);
    MX_USART1_UART_Init();

    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        lv_task_handler();
        HAL_Delay(5);
    }
}
