#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *led_pwr_active;
    lv_obj_t *led_ne;
    lv_obj_t *label_cut;
    lv_obj_t *label_mono;
    lv_obj_t *label_cut_mix_pwr;
    lv_obj_t *label_monocoag_pwr;
    lv_obj_t *btn_cut_mix_pwr_inc;
    lv_obj_t *label_cut_mix_pwr_inc;
    lv_obj_t *btn_cut_mix_pwr_dec;
    lv_obj_t *label_cut_mix_pwr_dec;
    lv_obj_t *btn_monocoag_pwr_inc;
    lv_obj_t *btn_monocoag_pwr_dec;
    lv_obj_t *label_bicoag_pwr;
    lv_obj_t *btn_bicoag_pwr_inc;
    lv_obj_t *label_bicoag_pwr_inc;
    lv_obj_t *btn_bicoag_pwr_dec;
    lv_obj_t *label_bicoag_pwr_dec;
    lv_obj_t *label_bi;
    lv_obj_t *label_coag;
    lv_obj_t *btn_cut_mode_sel;
    lv_obj_t *label_cut_mode_sel;
    lv_obj_t *btn_monocoag_mode_sel;
    lv_obj_t *label_monocoag_mode_sel;
    lv_obj_t *btn_bicoag_mode_sel;
    lv_obj_t *label_bicoag_mode_sel;
    lv_obj_t *btn_mix_mode_sel;
    lv_obj_t *label_mix_mode_sel;
    lv_obj_t *btn_mono_bi_sel;
    lv_obj_t *label_mono_bi_sel;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/