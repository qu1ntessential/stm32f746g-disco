#include "actions.h"
#include "ui.h"

#include "OsTasks.h"

extern ESG ESG15;

void action_cut_mix_pwr_inc(lv_event_t *e) {
    ESG15.changeCutMixPwr(true);
}

void action_cut_mix_pwr_dec(lv_event_t *e) {
    ESG15.changeCutMixPwr(false);
}

void action_monocoag_pwr_inc(lv_event_t *e) {
    ESG15.changeMonoCoagPwr(true);
}

void action_monocoag_pwr_dec(lv_event_t *e) {
    ESG15.changeMonoCoagPwr(false);
}

void action_bicoag_pwr_inc(lv_event_t *e) {
    ESG15.changeBiCoagPwr(true);
}

void action_bicoag_pwr_dec(lv_event_t *e) {
    ESG15.changeBiCoagPwr(false);
}

void action_cut_mode_sel(lv_event_t *e) {
    ESG15.changeCutMode();
    ESG15.syncUI();
}

void action_mix_mode_sel(lv_event_t *e) {
    ESG15.changeMixMode();
    ESG15.syncUI();
}

void action_monocoag_mode_sel(lv_event_t *e) {
    ESG15.changeMonoCoagMode();
    ESG15.syncUI();
}

void action_bicoag_mode_sel(lv_event_t *e) {
    ESG15.changeBiCoagMode();
    ESG15.syncUI();
}

void action_mono_bi_sel(lv_event_t *e) {
    ESG15.invMonoBiSel();
    ESG15.syncUI();
}

extern void action_rtn_main(lv_event_t *e) {
    loadScreen(SCREEN_ID_MAIN);

}

extern void action_load_page_settings(lv_event_t *e) {
    loadScreen(SCREEN_ID_SETTINGS);
}

extern void action_timeout_inc(lv_event_t *e) {
    ESG15.changeTimeout(true);
}

extern void action_timeout_dec(lv_event_t *e) {
    ESG15.changeTimeout(false);
}

extern void action_pause_twi(lv_event_t *e) {
    static bool pause_flag = false;

    pause_flag = !pause_flag;

    if (pause_flag) {
        lv_label_set_text(objects.label_pause_twi, "PAUSE");
        lv_obj_add_state(objects.btn_pause_twi, LV_STATE_CHECKED);
        vTaskSuspend(TwiTaskHandle);
    } else {
        lv_label_set_text(objects.label_pause_twi, "RUN");
        lv_obj_remove_state(objects.btn_pause_twi, LV_STATE_CHECKED);
        vTaskResume(TwiTaskHandle);
    }
}

extern void action_test(lv_event_t *e) {
    // 1. Создаем объект изображения
    lv_obj_t *img = lv_img_create(lv_scr_act());
    if(img == NULL) {
        print_log(ERROR_LOG, "Error: Failed to create image object\n");
        return;
    }

    // 2. Устанавливаем источник (без проверки возвращаемого значения)
    lv_img_set_src(img, "A:/1.bmp");

    // 3. Альтернативная проверка через свойство объекта
    if(lv_img_get_src(img) == NULL) {
        print_log(ERROR_LOG, "Error: Failed to load image\n");
        lv_obj_del(img);
        return;
    }

    // 4. Настраиваем позицию и обновляем экран
    lv_obj_set_pos(img, 0, 0);
    lv_refr_now(NULL);
}