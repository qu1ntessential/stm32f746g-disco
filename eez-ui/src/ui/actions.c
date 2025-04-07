#include "ui.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "ui_con.h"

extern objects_t objects;

extern QueueHandle_t uiEventQueue;
extern QueueHandle_t uiCmdQueue;


static void prepareEventItem(lv_event_t *e) {
    if (!e) {
        print_log(ERROR_LOG, "Invalid event\r\n");
        return;
    }

    UIEvent_t event = {
            .event_code = e->code,
            .widget_id = (WidgetID) user_data,
            .data = {.int_val = (e->code == LV_EVENT_LONG_PRESSED_REPEAT) ? 1 : 0},
            .timestamp = xTaskGetTickCount()
    };

    if (xQueueSend(uiEventQueue, &event, pdMS_TO_TICKS(100)) != pdPASS) {
        print_log(ERROR_LOG, "Queue full for widget %d\r\n", event.widget_id);
    }
}

void action_cut_mix_pwr_inc(lv_event_t *e) {

}

void action_cut_mix_pwr_dec(lv_event_t *e) {
}

void action_monocoag_pwr_inc(lv_event_t *e) {
}

void action_monocoag_pwr_dec(lv_event_t *e) {
}

void action_bicoag_pwr_inc(lv_event_t *e) {
}

void action_bicoag_pwr_dec(lv_event_t *e) {
}

void action_cut_mode_sel(lv_event_t *e) {
}

void action_mix_mode_sel(lv_event_t *e) {
}

void action_monocoag_mode_sel(lv_event_t *e) {
}

void action_bicoag_mode_sel(lv_event_t *e) {
}

void action_mono_bi_sel(lv_event_t *e) {
}
