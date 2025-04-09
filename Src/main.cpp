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
    __HAL_DBGMCU_FREEZE_TIM6(); ///< Cortex-M7 erratum 3092511
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    SCB_EnableICache();
    SCB_EnableDCache();

    BSP_LED_Init(LED_GREEN);
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
    MX_USART1_UART_Init();
    MX_I2C1_Init();
    twi1.Init();

    ESG15.Init();
    //uSD.Init();

    lv_init();
    tft_init();
    touchpad_init();
    //lv_fs_fatfs_init();
    ui_init();

    ConfigureTimerForRunTimeStats();
    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {}
}

/**
 * Использование 32-bit таймера для сбора статистики FreeRTOS (configGENERATE_RUN_TIME_STATS == 1)
 */
void ConfigureTimerForRunTimeStats(void) {
    MX_TIM2_Init();
    HAL_TIM_Base_Start(&htim2);
}

uint32_t GetRuntimeCounterValue(void) {
    return __HAL_TIM_GET_COUNTER(&htim2);
}