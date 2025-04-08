#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 272);
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_cut
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_cut = obj;
            lv_obj_set_pos(obj, 10, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Cut");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_mono
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_mono = obj;
            lv_obj_set_pos(obj, 130, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "MONO");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_cut_mix_pwr
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_cut_mix_pwr = obj;
            lv_obj_set_pos(obj, 10, 100);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_monocoag_pwr
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_monocoag_pwr = obj;
            lv_obj_set_pos(obj, 130, 100);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_cut_mix_pwr_inc
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_cut_mix_pwr_inc = obj;
            lv_obj_set_pos(obj, 70, 100);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_inc, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_inc, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_cut_mix_pwr_inc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_cut_mix_pwr_inc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "+");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_cut_mix_pwr_dec
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_cut_mix_pwr_dec = obj;
            lv_obj_set_pos(obj, 70, 150);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_dec, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_dec, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_cut_mix_pwr_dec
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_cut_mix_pwr_dec = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "-");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_monocoag_pwr_inc
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_monocoag_pwr_inc = obj;
            lv_obj_set_pos(obj, 200, 100);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_inc, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_inc, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "+");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_monocoag_pwr_dec
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_monocoag_pwr_dec = obj;
            lv_obj_set_pos(obj, 200, 150);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_dec, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_dec, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "-");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // label_bicoag_pwr
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_bicoag_pwr = obj;
            lv_obj_set_pos(obj, 280, 100);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_bicoag_pwr_inc
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_bicoag_pwr_inc = obj;
            lv_obj_set_pos(obj, 380, 100);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_inc, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_inc, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_bicoag_pwr_inc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_bicoag_pwr_inc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "+");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_bicoag_pwr_dec
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_bicoag_pwr_dec = obj;
            lv_obj_set_pos(obj, 380, 150);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_dec, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_dec, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_bicoag_pwr_dec
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_bicoag_pwr_dec = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "-");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // label_bi
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_bi = obj;
            lv_obj_set_pos(obj, 280, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "BI");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_coag
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_coag = obj;
            lv_obj_set_pos(obj, 210, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Coag");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_cut_mode_sel
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_cut_mode_sel = obj;
            lv_obj_set_pos(obj, 70, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_cut_mode_sel, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_cut_mode_sel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_cut_mode_sel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_monocoag_mode_sel
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_monocoag_mode_sel = obj;
            lv_obj_set_pos(obj, 200, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_monocoag_mode_sel, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_monocoag_mode_sel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_monocoag_mode_sel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_bicoag_mode_sel
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_bicoag_mode_sel = obj;
            lv_obj_set_pos(obj, 380, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_bicoag_mode_sel, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_bicoag_mode_sel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_bicoag_mode_sel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_mix_mode_sel
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_mix_mode_sel = obj;
            lv_obj_set_pos(obj, 20, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_mix_mode_sel, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_mix_mode_sel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_mix_mode_sel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_mono_bi_sel
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_mono_bi_sel = obj;
            lv_obj_set_pos(obj, 360, 0);
            lv_obj_set_size(obj, 120, 40);
            lv_obj_add_event_cb(obj, action_mono_bi_sel, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_event_cb(obj, action_mono_bi_sel, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_mono_bi_sel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_mono_bi_sel = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    {
        const char *new_val = get_var_cut_mix_pwr_str();
        const char *cur_val = lv_label_get_text(objects.label_cut_mix_pwr);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_cut_mix_pwr;
            lv_label_set_text(objects.label_cut_mix_pwr, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_monocoag_pwr_str();
        const char *cur_val = lv_label_get_text(objects.label_monocoag_pwr);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_monocoag_pwr;
            lv_label_set_text(objects.label_monocoag_pwr, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_bicoag_pwr_str();
        const char *cur_val = lv_label_get_text(objects.label_bicoag_pwr);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_bicoag_pwr;
            lv_label_set_text(objects.label_bicoag_pwr, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_cut_mode_str();
        const char *cur_val = lv_label_get_text(objects.label_cut_mode_sel);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_cut_mode_sel;
            lv_label_set_text(objects.label_cut_mode_sel, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_monocoag_mode_str();
        const char *cur_val = lv_label_get_text(objects.label_monocoag_mode_sel);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_monocoag_mode_sel;
            lv_label_set_text(objects.label_monocoag_mode_sel, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_bicoag_mode_str();
        const char *cur_val = lv_label_get_text(objects.label_bicoag_mode_sel);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_bicoag_mode_sel;
            lv_label_set_text(objects.label_bicoag_mode_sel, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_mix_mode_str();
        const char *cur_val = lv_label_get_text(objects.label_mix_mode_sel);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_mix_mode_sel;
            lv_label_set_text(objects.label_mix_mode_sel, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_mono_bi_sel_str();
        const char *cur_val = lv_label_get_text(objects.label_mono_bi_sel);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_mono_bi_sel;
            lv_label_set_text(objects.label_mono_bi_sel, new_val);
            tick_value_change_obj = NULL;
        }
    }
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
