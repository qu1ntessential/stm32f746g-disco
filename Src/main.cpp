#include "OsTasks.h"

#include "lvgl_port_tft.h"
#include "lvgl_port_touchpad.h"

#include "ff.h"
#include "sd_diskio_dma.h"

#include "FatFsWrapper.hpp"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);
extern "C" int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, HAL_MAX_DELAY);
    return ch;
}

extern const Diskio_drvTypeDef SD_Driver;
FatFsWrapper uSD(&SD_Driver);

int main() {
    HAL_Init();

    SystemClock_Config();
    PeriphCommonClock_Config();

    SCB_EnableICache();
    SCB_EnableDCache();

    BSP_LED_Init(LED_GREEN);
    MX_USART1_UART_Init();

    uSD.Init();

    lv_init();
    tft_init();
    touchpad_init();
    lv_fs_fatfs_init();
    ui_init();

    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        lv_task_handler();
        HAL_Delay(5);
    }
}
