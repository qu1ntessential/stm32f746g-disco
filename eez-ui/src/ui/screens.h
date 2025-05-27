#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *settings;
    lv_obj_t *led_coag;
    lv_obj_t *led_cut;
    lv_obj_t *led_ne;
    lv_obj_t *led_mix;
    lv_obj_t *led_ne_1;
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
    lv_obj_t *btn_settings;
    lv_obj_t *label_settings;
    lv_obj_t *btn_test;
    lv_obj_t *label_rtn_2;
    lv_obj_t *btn_rtn;
    lv_obj_t *label_rtn;
    lv_obj_t *label_timeout;
    lv_obj_t *btn_timeout_inc;
    lv_obj_t *label_cut_mix_pwr_inc_1;
    lv_obj_t *btn_timeout_dec;
    lv_obj_t *label_cut_mix_pwr_dec_1;
    lv_obj_t *btn_pause_twi;
    lv_obj_t *label_pause_twi;
    lv_obj_t *slider_volume;
    lv_obj_t *label_volume;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SETTINGS = 2,
};

void create_screen_main();
void tick_screen_main();

void create_screen_settings();
void tick_screen_settings();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/