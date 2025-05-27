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

static void event_handler_cb_main_led_coag(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
}

static void event_handler_cb_main_led_cut(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
}

static void event_handler_cb_main_led_ne(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
}

static void event_handler_cb_main_led_mix(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
}

static void event_handler_cb_main_led_ne_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
}

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
            lv_obj_set_pos(obj, 50, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Cut");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_mono
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_mono = obj;
            lv_obj_set_pos(obj, 170, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "MONO");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_cut_mix_pwr
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_cut_mix_pwr = obj;
            lv_obj_set_pos(obj, 50, 100);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_monocoag_pwr
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_monocoag_pwr = obj;
            lv_obj_set_pos(obj, 170, 100);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_cut_mix_pwr_inc
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_cut_mix_pwr_inc = obj;
            lv_obj_set_pos(obj, 110, 100);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_inc, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_inc, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 110, 150);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_dec, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_cut_mix_pwr_dec, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 240, 100);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_inc, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_inc, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 240, 150);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_dec, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_monocoag_pwr_dec, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 320, 100);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_bicoag_pwr_inc
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_bicoag_pwr_inc = obj;
            lv_obj_set_pos(obj, 420, 100);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_inc, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_inc, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 420, 150);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_dec, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_bicoag_pwr_dec, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 320, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "BI");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_coag
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_coag = obj;
            lv_obj_set_pos(obj, 250, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Coag");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_cut_mode_sel
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_cut_mode_sel = obj;
            lv_obj_set_pos(obj, 110, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_cut_mode_sel, LV_EVENT_CLICKED, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 240, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_monocoag_mode_sel, LV_EVENT_CLICKED, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 420, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_bicoag_mode_sel, LV_EVENT_CLICKED, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_set_pos(obj, 60, 200);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_mix_mode_sel, LV_EVENT_CLICKED, (void *)0);
            add_style_eleps(obj);
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
            lv_obj_add_event_cb(obj, action_mono_bi_sel, LV_EVENT_CLICKED, (void *)0);
            add_style_eleps(obj);
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
        {
            // led_coag
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.led_coag = obj;
            lv_obj_set_pos(obj, 2, 34);
            lv_obj_set_size(obj, 30, 30);
            lv_led_set_color(obj, lv_color_hex(0xff0000ff));
            lv_obj_add_event_cb(obj, event_handler_cb_main_led_coag, LV_EVENT_ALL, 0);
        }
        {
            // led_cut
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.led_cut = obj;
            lv_obj_set_pos(obj, 2, 2);
            lv_obj_set_size(obj, 30, 30);
            lv_led_set_color(obj, lv_color_hex(0xffffff00));
            lv_obj_add_event_cb(obj, event_handler_cb_main_led_cut, LV_EVENT_ALL, 0);
        }
        {
            // led_ne
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.led_ne = obj;
            lv_obj_set_pos(obj, 2, 130);
            lv_obj_set_size(obj, 30, 30);
            lv_led_set_color(obj, lv_color_hex(0xffff0000));
            lv_obj_add_event_cb(obj, event_handler_cb_main_led_ne, LV_EVENT_ALL, 0);
        }
        {
            // led_mix
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.led_mix = obj;
            lv_obj_set_pos(obj, 2, 66);
            lv_obj_set_size(obj, 30, 30);
            lv_led_set_color(obj, lv_color_hex(0xff00ff00));
            lv_obj_add_event_cb(obj, event_handler_cb_main_led_mix, LV_EVENT_ALL, 0);
        }
        {
            // led_ne_1
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.led_ne_1 = obj;
            lv_obj_set_pos(obj, 2, 98);
            lv_obj_set_size(obj, 30, 30);
            lv_led_set_color(obj, lv_color_hex(0xff00ffff));
            lv_obj_add_event_cb(obj, event_handler_cb_main_led_ne_1, LV_EVENT_ALL, 0);
        }
        {
            // btn_settings
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_settings = obj;
            lv_obj_set_pos(obj, 0, 232);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_load_page_settings, LV_EVENT_CLICKED, (void *)0);
            add_style_eleps(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_settings
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_settings = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "S");
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
    {
        int32_t new_val = get_var_led_coag_br();
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(objects.led_coag);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.led_coag;
            lv_led_set_brightness(objects.led_coag, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_led_cut_br();
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(objects.led_cut);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.led_cut;
            lv_led_set_brightness(objects.led_cut, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_led_alarm_br();
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(objects.led_ne);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.led_ne;
            lv_led_set_brightness(objects.led_ne, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_led_mix_br();
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(objects.led_mix);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.led_mix;
            lv_led_set_brightness(objects.led_mix, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_led_ne_br();
        if (new_val < 0) new_val = 0;
        else if (new_val > 255) new_val = 255;
        int32_t cur_val = lv_led_get_brightness(objects.led_ne_1);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.led_ne_1;
            lv_led_set_brightness(objects.led_ne_1, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 272);
    {
        lv_obj_t *parent_obj = obj;
        {
            // btn_rtn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_rtn = obj;
            lv_obj_set_pos(obj, 0, 232);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_rtn_main, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_rtn
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_rtn = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "R");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // label_timeout
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_timeout = obj;
            lv_obj_set_pos(obj, 20, 10);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_timeout_inc
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_timeout_inc = obj;
            lv_obj_set_pos(obj, 80, 10);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_timeout_inc, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_timeout_inc, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_cut_mix_pwr_inc_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_cut_mix_pwr_inc_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "+");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_timeout_dec
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_timeout_dec = obj;
            lv_obj_set_pos(obj, 80, 60);
            lv_obj_set_size(obj, 40, 40);
            lv_obj_add_event_cb(obj, action_timeout_dec, LV_EVENT_CLICKED, (void *)0);
            lv_obj_add_event_cb(obj, action_timeout_dec, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_cut_mix_pwr_dec_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_cut_mix_pwr_dec_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "-");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // btn_pause_twi
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_pause_twi = obj;
            lv_obj_set_pos(obj, 320, 60);
            lv_obj_set_size(obj, 120, 40);
            lv_obj_add_event_cb(obj, action_pause_twi, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_pause_twi
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_pause_twi = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "RUN");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // slider_volume
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.slider_volume = obj;
            lv_obj_set_pos(obj, 305, 136);
            lv_obj_set_size(obj, 150, 10);
            lv_slider_set_range(obj, 0, 3);
            lv_slider_set_value(obj, 1, LV_ANIM_ON);
        }
        {
            // label_volume
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_volume = obj;
            lv_obj_set_pos(obj, 280, 160);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_test
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_test = obj;
            lv_obj_set_pos(obj, 320, 10);
            lv_obj_set_size(obj, 120, 40);
            lv_obj_add_event_cb(obj, action_test, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_rtn_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_rtn_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "TEST");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
    {
        const char *new_val = get_var_timeout_str();
        const char *cur_val = lv_label_get_text(objects.label_timeout);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_timeout;
            lv_label_set_text(objects.label_timeout, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_volume_str();
        const char *cur_val = lv_label_get_text(objects.label_volume);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.label_volume;
            lv_label_set_text(objects.label_volume, new_val);
            tick_value_change_obj = NULL;
        }
    }
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_settings,
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
    create_screen_settings();
}
