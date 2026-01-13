#include "Main.hpp"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);

extern const Diskio_drvTypeDef SD_Driver;
FatFsWrapper uSD(&SD_Driver);

I2C twi1(&hi2c1);

/* Функции для сбора статистики времени выполнения */
extern "C" void ConfigureTimerForRunTimeStats(void) {
    MX_TIM2_Init();
    HAL_TIM_Base_Start(&htim2);
}

extern "C" uint32_t GetRuntimeCounterValue(void) {
    return __HAL_TIM_GET_COUNTER(&htim2);
}

int main() {
    __HAL_DBGMCU_FREEZE_TIM5();
    __HAL_DBGMCU_FREEZE_TIM6(); ///< Cortex-M7 erratum 3092511
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    SCB_EnableICache();
    SCB_EnableDCache();

    BSP_LED_Init(LED_GREEN);
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
    MX_TIM5_Init();
    MX_USART1_UART_Init();
    MX_USART6_UART_Init();
    MX_I2C1_Init();
    MX_DAC_Init();
    twi1.Init();

    lv_init();
    tft_init();
    touchpad_init();
    ui_init();

    ConfigureTimerForRunTimeStats();
    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {}
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        HAL_IncTick();
        lv_tick_inc(1);
    }
}
