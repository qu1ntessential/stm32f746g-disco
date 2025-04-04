#include "OsTasks.h"

#include "lvgl_port_tft.h"
#include "lvgl_port_touchpad.h"

#include "lv_demos.h"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);

int main() {
    HAL_Init();

    SystemClock_Config();
    PeriphCommonClock_Config();

    SCB_EnableICache();
    SCB_EnableDCache();

    lv_init();
    tft_init();
    touchpad_init();

    lv_demo_widgets();

    BSP_LED_Init(LED_GREEN);



    //FreeRTOS_Resources_Init();
    //vTaskStartScheduler();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        lv_task_handler();
        HAL_Delay(5);
    }
}
