#include "lvgl_port_touchpad.h"
#include "lvgl_port_tft.h"

static TS_StateTypeDef TS_State;

static void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);

/**
 * Initialize your input devices here
 */
void touchpad_init(void) {
    BSP_TS_Init(TFT_HOR_RES, TFT_VER_RES);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchpad_read);
}

/**
 * Read an input device
 * @param indev_id id of the input device to read
 * @param x put the x coordinate here
 * @param y put the y coordinate here
 * @return true: the device is pressed, false: released
 */
static void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    /* Read your touchpad */
    static int16_t last_x = 0;
    static int16_t last_y = 0;
    BSP_LED_Toggle(LED1);

    BSP_TS_GetState(&TS_State);
    if (TS_State.touchDetected) {
        data->point.x = TS_State.touchX[0];
        data->point.y = TS_State.touchY[0];
        last_x = data->point.x;
        last_y = data->point.y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
