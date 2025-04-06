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

FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
uint8_t workBuffer[_MAX_SS];

void fatfs_test();

int main() {
    HAL_Init();

    SystemClock_Config();
    PeriphCommonClock_Config();

    SCB_EnableICache();
    SCB_EnableDCache();

    BSP_LED_Init(LED_GREEN);
    MX_USART1_UART_Init();

    fatfs_test();

    lv_init();
    tft_init();
    touchpad_init();
    lv_fs_fatfs_init();
    ui_init();

    print_log(ERROR_LOG, "Hello world!\r\n");

    FreeRTOS_Resources_Init();
    vTaskStartScheduler();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        lv_task_handler();
        HAL_Delay(5);
    }
}

void fatfs_test() {
    FRESULT res;                                          /* FatFs function common result code */
    uint32_t byteswritten, bytesread;                     /* File write/read counts */
    uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
    uint8_t rtext[100];

    /*##-1- Link the micro SD disk I/O driver ##################################*/
    if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
        /*##-2- Register the file system object to the FatFs module ##############*/
        if (f_mount(&SDFatFs, (TCHAR const *) SDPath, 0) != FR_OK) {
            /* FatFs Initialization Error */
            Error_Handler();
        } else {
            /*##-3- Create a FAT file system (format) on the logical drive #########*/
            /* WARNING: Formatting the uSD card will delete all content on the device */
            print_log(DEBUG_LOG, "Creating filesystem...\r\n");
            if (f_mkfs((TCHAR const *) SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer)) != FR_OK) {
                /* FatFs Format Error */
                Error_Handler();
            } else {
                /*##-4- Create and Open a new text file object with write access #####*/
                if (f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
                    /* 'STM32.TXT' file Open for write Error */
                    Error_Handler();
                } else {
                    /*##-5- Write data to the text file ################################*/
                    res = f_write(&MyFile, wtext, sizeof(wtext), (UINT *) &byteswritten);

                    if ((byteswritten == 0) || (res != FR_OK)) {
                        /* 'STM32.TXT' file Write or EOF Error */
                        Error_Handler();
                    } else {
                        /*##-6- Close the open text file #################################*/
                        f_close(&MyFile);

                        /*##-7- Open the text file object with read access ###############*/
                        if (f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK) {
                            /* 'STM32.TXT' file Open for read Error */
                            Error_Handler();
                        } else {
                            /*##-8- Read data from the text file ###########################*/
                            res = f_read(&MyFile, rtext, sizeof(rtext), (UINT *) &bytesread);

                            if ((bytesread == 0) || (res != FR_OK)) {
                                /* 'STM32.TXT' file Read or EOF Error */
                                Error_Handler();
                            } else {
                                /*##-9- Close the open text file #############################*/
                                f_close(&MyFile);

                                /*##-10- Compare read data with the expected data ############*/
                                if ((bytesread != byteswritten)) {
                                    /* Read data is different from the expected data */
                                    Error_Handler();
                                } else {
                                    /* Success of the demo: no error occurrence */
                                    BSP_LED_On(LED1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //FATFS_UnLinkDriver(SDPath);
}