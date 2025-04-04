#ifndef LVGL_PORT_TOUCHPAD_H
#define LVGL_PORT_TOUCHPAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"

void touchpad_init(void);

#ifdef __cplusplus
}
#endif

#endif //LVGL_PORT_TOUCHPAD_H
