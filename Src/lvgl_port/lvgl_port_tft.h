#ifndef LVGL_PORT_TFT_H_H
#define LVGL_PORT_TFT_H_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

#include "lv_conf.h"
#include "lvgl.h"

#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_ts.h"
#include "rk043fn48h.h"

#define TFT_HOR_RES 480
#define TFT_VER_RES 272

void tft_init(void);

#ifdef __cplusplus
}
#endif

#endif //LVGL_PORT_TFT_H_H
