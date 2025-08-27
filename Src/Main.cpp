#include "Main.hpp"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_msc_storage.h"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);

extern const Diskio_drvTypeDef SD_Driver;
FatFsWrapper uSD(&SD_Driver);

I2C twi1(&hi2c1);

USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;

static void USB_FS_Init();

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

    /*
    lv_init();
    tft_init();
    touchpad_init();
    ui_init();
    */

    USB_FS_Init();

    ConfigureTimerForRunTimeStats();
    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {}
}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        HAL_IncTick();
        //lv_tick_inc(1);
    }
}

void USB_FS_Init() {
    USBD_StatusTypeDef l_stat;

    // HAL_PWREx_EnableUSBVoltageDetector();

    l_stat = USBD_Init(&USBD_Device, &MSC_Desc, 0);
    print_log(l_stat > 0 ? ERROR_LOG : DEBUG_LOG, "USBD_Init ended with a result %u\r\n", l_stat);

    l_stat = USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);
    print_log(l_stat > 0 ? ERROR_LOG : DEBUG_LOG, "USBD_RegisterClass ended with a result %u\r\n", l_stat);

    l_stat = static_cast<USBD_StatusTypeDef>(USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops));
    print_log(l_stat > 0 ? ERROR_LOG : DEBUG_LOG, "USBD_MSC_RegisterStorage ended with a result %u\r\n", l_stat);

    l_stat = USBD_Start(&USBD_Device);
    print_log(l_stat > 0 ? ERROR_LOG : DEBUG_LOG, "USBD_Start ended with a result %u\r\n", l_stat);
}