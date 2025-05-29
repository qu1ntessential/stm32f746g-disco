/*
 * lvgl_port_tft.c — порт для LVGL с поддержкой LTDC и DMA2D (STM32)
 * Обновлено: поддержка LV_COLOR_DEPTH == 32 (ARGB8888) и DMA2D с колбэками через поля hdma2d.XferCpltCallback
 */

#include "lvgl.h"
#include "lvgl_port_tft.h"
#include "stm32f7xx_hal.h"
#include <string.h>

#define TFT_HOR_RES 480
#define TFT_VER_RES 272

#define MY_FB_SECTION __attribute__((section(".lvgl_buffer"), aligned(8)))

static lv_display_t *display;
static uint32_t my_fb[TFT_HOR_RES * TFT_VER_RES] MY_FB_SECTION;

static int32_t x1_flush, x2_flush;
static int32_t y1_flush, y2_fill;
static int32_t y_fill_act;
static const lv_area_t *area_to_flush;
static const lv_color32_t *buf_to_flush;
static lv_display_t *disp_to_flush;

static DMA2D_HandleTypeDef hdma2d;

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

static void start_dma2d_line(void);

static void dma2d_init(void);

static void MyDMA2D_TransferCompleteCallback(DMA2D_HandleTypeDef *hdma2d);

static void MyDMA2D_TransferErrorCallback(DMA2D_HandleTypeDef *hdma2d);

void tft_init(void) {
    /* LCD Initialization */
    BSP_LCD_Init();
    dma2d_init();

    uint32_t i;
    for (i = 0; i < (TFT_HOR_RES * TFT_VER_RES); i++) {
        my_fb[i] = 0;
    }

    /* LCD Initialization */
    BSP_LCD_LayerDefaultInit(0, (uint32_t) my_fb);
    //LCD_LayerRgb565Init((uint32_t) my_fb);

    /* Enable the LCD */
    BSP_LCD_DisplayOn();
    //DMA_Config();

    static uint32_t buf1[TFT_HOR_RES * TFT_VER_RES] MY_FB_SECTION;
    static uint32_t buf2[TFT_HOR_RES * TFT_VER_RES] MY_FB_SECTION;

    display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_buffers(display, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_cb);
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    x1_flush = LV_CLAMP(0, area->x1, TFT_HOR_RES - 1);
    x2_flush = LV_CLAMP(0, area->x2, TFT_HOR_RES - 1);
    y1_flush = LV_CLAMP(0, area->y1, TFT_VER_RES - 1);
    y2_fill = LV_CLAMP(0, area->y2, TFT_VER_RES - 1);
    y_fill_act = y1_flush;

    area_to_flush = area;
    buf_to_flush = (const lv_color32_t *) px_map;
    disp_to_flush = disp;

    start_dma2d_line();
}

static void start_dma2d_line(void) {
    if (y_fill_act > y2_fill) {
        lv_disp_flush_ready(disp_to_flush);
        return;
    }

    uint32_t width = x2_flush - x1_flush + 1;
    const uint32_t *src = (const uint32_t *) buf_to_flush;
    uint32_t *dst = &my_fb[y_fill_act * TFT_HOR_RES + x1_flush];

    SCB_CleanDCache_by_Addr((uint32_t *) src, width * sizeof(uint32_t));

    HAL_DMA2D_DeInit(&hdma2d);
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M;
    hdma2d.Init.ColorMode = DMA2D_ARGB8888;
    hdma2d.Init.OutputOffset = TFT_HOR_RES - width;

    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
    hdma2d.LayerCfg[1].InputOffset = 0;
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;

    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);

    HAL_DMA2D_RegisterCallback(&hdma2d, HAL_DMA2D_TRANSFERCOMPLETE_CB_ID, MyDMA2D_TransferCompleteCallback);
    HAL_DMA2D_RegisterCallback(&hdma2d, HAL_DMA2D_TRANSFERERROR_CB_ID, MyDMA2D_TransferErrorCallback);

    HAL_DMA2D_Start_IT(&hdma2d, (uint32_t) src, (uint32_t) dst, width, 1);
}

static void MyDMA2D_TransferCompleteCallback(DMA2D_HandleTypeDef *hdma2d) {
    y_fill_act++;
    buf_to_flush += (x2_flush - x1_flush + 1);
    start_dma2d_line();
}

static void MyDMA2D_TransferErrorCallback(DMA2D_HandleTypeDef *hdma2d) {
    while (1); // Остановка на ошибке DMA2D
}

static void dma2d_init(void) {
    __HAL_RCC_DMA2D_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA2D_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);
}

void DMA2D_IRQHandler(void) {
    HAL_DMA2D_IRQHandler(&hdma2d);
}
