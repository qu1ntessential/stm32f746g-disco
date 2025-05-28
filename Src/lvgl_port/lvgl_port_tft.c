#include "lvgl_port_tft.h"

#if LV_COLOR_DEPTH != 16 && LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32
#error LV_COLOR_DEPTH must be 16, 24, or 32
#endif

#define CPY_BUF_DMA_STREAM               DMA2_Stream0
#define CPY_BUF_DMA_CHANNEL              DMA_CHANNEL_0
#define CPY_BUF_DMA_STREAM_IRQ           DMA2_Stream0_IRQn
#define CPY_BUF_DMA_STREAM_IRQHANDLER    DMA2_Stream0_IRQHandler

/*These 3 functions are needed by LittlevGL*/
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

static void LCD_LayerRgb565Init(uint32_t FB_Address);

static void DMA_Config(void);

static void DMA_TransferComplete(DMA_HandleTypeDef *han);

static void DMA_TransferError(DMA_HandleTypeDef *han);

extern LTDC_HandleTypeDef hLtdcHandler;
static lv_display_t *display;

#if LV_COLOR_DEPTH == 16
typedef uint16_t uintpixel_t;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
typedef uint32_t uintpixel_t;
#endif

/* You can try to change buffer to internal ram by uncommenting line below and commenting
 * SDRAM one. */
static uintpixel_t my_fb[TFT_HOR_RES * TFT_VER_RES]
        __attribute__((section(".lvgl_buffer"), aligned(4)));

//static __IO uintpixel_t *my_fb = (__IO uintpixel_t *) (0xC0701000);

static DMA_HandleTypeDef DmaHandle;
static int32_t x1_flush;
static int32_t y1_flush;
static int32_t x2_flush;
static int32_t y2_fill;
static int32_t y_fill_act;
static const uint16_t *buf_to_flush;

static lv_disp_t *our_disp = NULL;

void tft_init(void) {
    /* There is only one display on STM32 */
    if (our_disp != NULL)
        abort();
    /* LCD Initialization */
    BSP_LCD_Init();

    uint32_t i;
    for (i = 0; i < (TFT_HOR_RES * TFT_VER_RES); i++) {
        my_fb[i] = 0;
    }

    /* LCD Initialization */
    LCD_LayerRgb565Init((uint32_t) my_fb);

    /* Enable the LCD */
    BSP_LCD_DisplayOn();

    DMA_Config();

    static uint16_t buf1[TFT_HOR_RES * TFT_VER_RES]
            __attribute__((section(".lvgl_buffer"), aligned(4)));
    static uint16_t buf2[TFT_HOR_RES * TFT_VER_RES]
            __attribute__((section(".lvgl_buffer"), aligned(4)));
    display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_buffers(display, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_cb);
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    int32_t x1 = area->x1;
    int32_t x2 = area->x2;
    int32_t y1 = area->y1;
    int32_t y2 = area->y2;
    /*Return if the area is out the screen*/

    if (x2 < 0) return;
    if (y2 < 0) return;
    if (x1 > TFT_HOR_RES - 1) return;
    if (y1 > TFT_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > TFT_HOR_RES - 1 ? TFT_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > TFT_VER_RES - 1 ? TFT_VER_RES - 1 : y2;

    x1_flush = act_x1;
    y1_flush = act_y1;
    x2_flush = act_x2;
    y2_fill = act_y2;
    y_fill_act = act_y1;
    buf_to_flush = (uint16_t *) px_map;

    SCB_CleanInvalidateDCache();
    SCB_InvalidateICache();

    /*##-7- Start the DMA transfer using the interrupt mode #*/
    /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
    /* Enable All the DMA interrupts */
    HAL_StatusTypeDef err;
    uint32_t length = (x2_flush - x1_flush + 1);
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
    err = HAL_DMA_Start_IT(&DmaHandle, (uint32_t) buf_to_flush, (uint32_t) &my_fb[y_fill_act * TFT_HOR_RES + x1_flush],
                           length);
    if (err != HAL_OK) {
        while (1);    /*Halt on error*/
    }
}

static void LCD_LayerRgb565Init(uint32_t FB_Address) {
    LTDC_LayerCfgTypeDef layer_cfg;

    /* Layer Init */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = TFT_HOR_RES;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = TFT_VER_RES;

#if LV_COLOR_DEPTH == 16
    layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
#else
#error Unsupported color depth (see tft.c)
#endif
    layer_cfg.FBStartAdress = FB_Address;
    layer_cfg.Alpha = 255;
    layer_cfg.Alpha0 = 0;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = TFT_HOR_RES;
    layer_cfg.ImageHeight = TFT_VER_RES;

    HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, 0);
}

static void DMA_Config(void) {
    /*## -1- Enable DMA2 clock #################################################*/
    __HAL_RCC_DMA2_CLK_ENABLE();

    /*##-2- Select the DMA functional Parameters ###############################*/
    DmaHandle.Init.Channel = CPY_BUF_DMA_CHANNEL;                   /* DMA_CHANNEL_0                    */
    DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;                /* M2M transfer mode                */
    DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;                     /* Peripheral increment mode Enable */
    DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                        /* Memory increment mode Enable     */
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Peripheral data alignment : 16bit */
    DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;      /* memory data alignment : 16bit     */
    DmaHandle.Init.Mode = DMA_NORMAL;                               /* Normal DMA mode                  */
    DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;                    /* priority level : high            */
    DmaHandle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;                  /* FIFO mode enabled                */
    DmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL; /* FIFO threshold: 1/4 full   */
    DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;                    /* Memory burst                     */
    DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;                 /* Peripheral burst                 */

    /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
    DmaHandle.Instance = CPY_BUF_DMA_STREAM;

    /*##-4- Initialize the DMA stream ##########################################*/
    if (HAL_DMA_Init(&DmaHandle) != HAL_OK) {
        while (1) {
        }
    }

    /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);
    HAL_DMA_RegisterCallback(&DmaHandle, HAL_DMA_XFER_ERROR_CB_ID, DMA_TransferError);

    /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
    HAL_NVIC_SetPriority(CPY_BUF_DMA_STREAM_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(CPY_BUF_DMA_STREAM_IRQ);
}

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void DMA_TransferComplete(DMA_HandleTypeDef *han) {
    y_fill_act++;

    if (y_fill_act > y2_fill) {
        SCB_CleanInvalidateDCache();
        SCB_InvalidateICache();
        lv_disp_flush_ready(display);
    } else {
        uint32_t length = (x2_flush - x1_flush + 1);
        buf_to_flush += x2_flush - x1_flush + 1;
        /*##-7- Start the DMA transfer using the interrupt mode ####################*/
        /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
        /* Enable All the DMA interrupts */
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
        length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
        if (HAL_DMA_Start_IT(han, (uint32_t) buf_to_flush, (uint32_t) &my_fb[y_fill_act * TFT_HOR_RES + x1_flush],
                             length) != HAL_OK) {
            while (1);    /*Halt on error*/
        }
    }
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void DMA_TransferError(DMA_HandleTypeDef *han) {

}

/**
  * @brief  This function handles DMA Stream interrupt request.
  * @param  None
  * @retval None
  */
void CPY_BUF_DMA_STREAM_IRQHANDLER(void) {
    /* Check the interrupt and clear flag */
    HAL_DMA_IRQHandler(&DmaHandle);
}
