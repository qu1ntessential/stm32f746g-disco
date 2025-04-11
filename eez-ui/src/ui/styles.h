#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: eleps
lv_style_t *get_style_eleps_MAIN_DEFAULT();
lv_style_t *get_style_eleps_MAIN_DISABLED();
void add_style_eleps(lv_obj_t *obj);
void remove_style_eleps(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/