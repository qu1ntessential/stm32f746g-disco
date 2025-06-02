#include "OsTasks.h"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);

extern const Diskio_drvTypeDef SD_Driver;
FatFsWrapper uSD(&SD_Driver);

I2C twi1(&hi2c1);

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
    MX_USART6_UART_Init();
    MX_I2C1_Init();
    twi1.Init();
    BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 80, 44100);
    BSP_AUDIO_OUT_Play((uint16_t *) audio_file, audio_file_size);

    lv_init();
    tft_init();
    touchpad_init();
    ui_init();

    ConfigureTimerForRunTimeStats();
    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {}
}

void ConfigureTimerForRunTimeStats(void) {
    MX_TIM2_Init();
    HAL_TIM_Base_Start(&htim2);
}

uint32_t GetRuntimeCounterValue(void) {
    return __HAL_TIM_GET_COUNTER(&htim2);
}