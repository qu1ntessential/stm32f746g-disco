#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_cut_mix_pwr_inc(lv_event_t * e);
extern void action_cut_mix_pwr_dec(lv_event_t * e);
extern void action_monocoag_pwr_inc(lv_event_t * e);
extern void action_monocoag_pwr_dec(lv_event_t * e);
extern void action_bicoag_pwr_inc(lv_event_t * e);
extern void action_bicoag_pwr_dec(lv_event_t * e);
extern void action_cut_mode_sel(lv_event_t * e);
extern void action_mix_mode_sel(lv_event_t * e);
extern void action_monocoag_mode_sel(lv_event_t * e);
extern void action_bicoag_mode_sel(lv_event_t * e);
extern void action_mono_bi_sel(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/