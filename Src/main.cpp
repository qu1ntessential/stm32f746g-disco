#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dcmi.h"
#include "dma2d.h"
#include "eth.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "quadspi.h"
#include "rtc.h"
#include "sai.h"
#include "sdmmc.h"
#include "spdifrx.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"

#include "uart_log.h"

extern "C" void SystemClock_Config(void);
extern "C" void PeriphCommonClock_Config(void);
extern "C" void MX_FREERTOS_Init(void);

int main() {
    HAL_Init();

    SystemClock_Config();
    PeriphCommonClock_Config();

    MX_GPIO_Init();
    //MX_ADC3_Init();
    //MX_CRC_Init();
    //MX_DCMI_Init();
    //MX_DMA2D_Init();
    //MX_ETH_Init();
    //MX_FMC_Init();
    //MX_I2C1_Init();
    //MX_I2C3_Init();
    //MX_LTDC_Init();
    //MX_QUADSPI_Init();
    //MX_RTC_Init();
    //MX_SAI2_Init();
    //MX_SDMMC1_SD_Init();
    //MX_SPDIFRX_Init();
    //MX_SPI2_Init();
    //MX_TIM1_Init();
    //MX_TIM2_Init();
    //MX_TIM3_Init();
    //MX_TIM5_Init();
    //MX_TIM8_Init();
    //MX_TIM12_Init();
    MX_USART1_UART_Init();   // Virtual COM Port
    //MX_USART6_UART_Init();
    //MX_FATFS_Init();


    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    MX_FREERTOS_Init();
    osKernelStart();
    while (1) {}
}
