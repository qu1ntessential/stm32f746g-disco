#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BIMIX_BICOAG,
    BICUT_BICOAG = 0b010,
    MONOMIX_BICOAG = 0b100,
    MONOMIX_MONOCOAG = 0b101,
    MONOCUT_BICOAG = 0b110,
    MONOCUT_MONOCOAG = 0b111
} selActMode_t;

typedef struct esgParam {
    // true - mono active, false - bi active
    bool mono_bi_sel: 1;
    // true - cut active, false - mix active
    bool cut_mix_sel: 1;
    // true - monocoag active, false - bicoag active
    bool mono_bi_coag_sel: 1;
    uint16_t monocut_0: 9;
    uint16_t monocut_1: 9;
    uint16_t monocut_2: 9;
    // active monocut_mode (0..2)
    uint16_t monocut_mode: 2;
    uint16_t monomix_0: 9;
    uint16_t monomix_1: 9;
    uint16_t monomix_2: 9;
    // active monomix_mode (0..2)
    uint16_t monomix_mode: 2;
    uint16_t : 3;
    uint16_t monocoag_0: 9;
    uint16_t monocoag_1: 9;
    uint16_t monocoag_2: 9;
    // active monocoag_mode (0..2)
    uint16_t monocoag_mode: 2;
    uint16_t : 3;
    uint16_t bicut_0: 9;
    uint16_t bicut_1: 9;
    uint16_t bicut_2: 9;
    uint16_t bicut_mode: 2;
    uint16_t : 3;
    uint16_t bimix_0: 9;
    uint16_t bimix_1: 9;
    uint16_t bimix_2: 9;
    uint16_t bimix_mode: 2;
    uint16_t : 3;
    uint16_t bicoag_0: 9;
    uint16_t bicoag_1: 9;
    uint16_t bicoag_2: 9;
    // active bicaog_mode (0..2)
    uint16_t bicoag_mode: 2;
    uint16_t : 3;
    int32_t led_pwr_active_color;
    int32_t led_pwr_active_br;
    int32_t led_ne_br;
} esgParam_t;

bool changePwr(uint8_t select, uint8_t regime, uint8_t step, bool isInc);

bool changeMode(uint8_t select);

void getCutMixSel(const bool *sel_flag, const uint8_t *mode_flag);

void getMonoBiCoagSel(const bool *sel_flag, const uint8_t *mode_flag);

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
    FLOW_GLOBAL_VARIABLE_MONO_BI_SEL_STR = 7
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


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/