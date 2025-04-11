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
}

void action_mix_mode_sel(lv_event_t *e) {
    ESG15.changeMixMode();
}

void action_monocoag_mode_sel(lv_event_t *e) {
    ESG15.changeMonoCoagMode();
}

void action_bicoag_mode_sel(lv_event_t *e) {
    ESG15.changeBiCoagMode();
}

void action_mono_bi_sel(lv_event_t *e) {
    ESG15.invMonoBiSel();
    ESG15.syncUI();
    vTaskDelay(pdMS_TO_TICKS(500));
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

    if (pause_flag) vTaskSuspend(TwiTaskHandle);
    else vTaskResume(TwiTaskHandle);
}