#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// enum declarations


// Flow global variables
enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_CUT_MIX_PWR_STR = 0,
    FLOW_GLOBAL_VARIABLE_MONOCOAG_PWR_STR = 1,
    FLOW_GLOBAL_VARIABLE_BICOAG_PWR_STR = 2,
    FLOW_GLOBAL_VARIABLE_CUT_MODE_STR = 3,
    FLOW_GLOBAL_VARIABLE_MONOCOAG_MODE_STR = 4,
    FLOW_GLOBAL_VARIABLE_BICOAG_MODE_STR = 5,
    FLOW_GLOBAL_VARIABLE_MIX_MODE_STR = 6,
    FLOW_GLOBAL_VARIABLE_MONO_BI_SEL_STR = 7,
    FLOW_GLOBAL_VARIABLE_LED_CUT_BR = 8
};

// Native global variables
extern const char *get_var_cut_mix_pwr_str();
extern void set_var_cut_mix_pwr_str(const char *value);
extern const char *get_var_monocoag_pwr_str();
extern void set_var_monocoag_pwr_str(const char *value);
extern const char *get_var_bicoag_pwr_str();
extern void set_var_bicoag_pwr_str(const char *value);
extern const char *get_var_cut_mode_str();
extern void set_var_cut_mode_str(const char *value);
extern const char *get_var_monocoag_mode_str();
extern void set_var_monocoag_mode_str(const char *value);
extern const char *get_var_bicoag_mode_str();
extern void set_var_bicoag_mode_str(const char *value);
extern const char *get_var_mix_mode_str();
extern void set_var_mix_mode_str(const char *value);
extern const char *get_var_mono_bi_sel_str();
extern void set_var_mono_bi_sel_str(const char *value);
extern int32_t get_var_led_cut_br();
extern void set_var_led_cut_br(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/