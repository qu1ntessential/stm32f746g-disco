#include "vars.h"
#include "ESG.hpp"

extern ESG ESG15;
static char str[5];

const char *get_var_cut_mix_pwr_str() {
    snprintf(str, sizeof(str), "%hu", ESG15.getCutMixPower());
    return str;
}

const char *get_var_cut_mode_str() {
    snprintf(str, sizeof(str), "%u", ESG15.getCutMode());
    return str;
}

const char *get_var_bicoag_pwr_str() {
    snprintf(str, sizeof(str), "%hu", ESG15.getBiCoagPower());
    return str;
}

const char *get_var_bicoag_mode_str() {
    snprintf(str, sizeof(str), "%u", ESG15.getBiCoagMode());
    return str;
}

const char *get_var_mix_mode_str() {
    snprintf(str, sizeof(str), "%u", ESG15.getMixMode());
    return str;
}

const char *get_var_monocoag_pwr_str() {
    snprintf(str, sizeof(str), "%hu", ESG15.getMonoCoagPower());
    return str;
}

const char *get_var_monocoag_mode_str() {
    snprintf(str, sizeof(str), "%u", ESG15.getMonoCoagMode());
    return str;
}

const char *get_var_mono_bi_sel_str() {
    if (ESG15.getMonoBiFlag()) {
        snprintf(str, sizeof(str), "MONO");
        return str;
    } else {
        snprintf(str, sizeof(str), "BI");
        return str;
    }
}

int32_t get_var_led_cut_br() {
    if (ESG15.getStateUI().isCutActive) return 255;
    else return 0;
}

extern int32_t get_var_led_mix_br() {
    if (ESG15.getStateUI().isMixActive) return 255;
    else return 0;
}

extern int32_t get_var_led_coag_br() {
    if (ESG15.getStateUI().isCoagActive) return 255;
    else return 0;
}

extern int32_t get_var_led_alarm_br() {
    if (ESG15.getStateUI().isCrash) return 255;
    else return 0;
}

extern int32_t get_var_led_ne_br() {
    if (ESG15.getStateUI().isNE) return 255;
    else return 0;
}