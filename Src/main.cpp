#include "OsTasks.h"

#include "lvgl_port_tft.h"
#include "lvgl_port_touchpad.h"

#include "ff.h"
#include "sd_diskio_dma.h"

#include "FatFsWrapper.hpp"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);

//extern const Diskio_drvTypeDef SD_Driver;
//FatFsWrapper uSD(&SD_Driver);

I2C twi1(&hi2c1);
ESG ESG15(&twi1);

int main() {
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    SCB_EnableICache();
    SCB_EnableDCache();

    BSP_LED_Init(LED_GREEN);
    MX_USART1_UART_Init();
    MX_I2C1_Init();
    twi1.Init();

    //uSD.Init();

    lv_init();
    tft_init();
    touchpad_init();
    //lv_fs_fatfs_init();
    ui_init();

    print_log(DEBUG_LOG, "1234\r\n");

    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        lv_task_handler();
        HAL_Delay(5);
    }
}
