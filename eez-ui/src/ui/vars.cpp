#include "vars.h"
#include "ESG.hpp"

extern ESG ESG15;
static char str[20];

const char *get_var_cut_mix_pwr_str() {
    ESG15.getMonoCutPower(str, 20);
    return str;
}

void set_var_cut_mix_pwr_str(const char *value) {}

const char *get_var_cut_mode_str() {

}

void set_var_cut_mode_str(const char *value) {}

const char *get_var_bicoag_pwr_str() {

}

void set_var_bicoag_pwr_str(const char *value) {}

const char *get_var_bicoag_mode_str() {

}

void set_var_bicoag_mode_str(const char *value) {}

const char *get_var_mix_mode_str() {

}

void set_var_mix_mode_str(const char *value) {}

const char *get_var_monocoag_pwr_str() {

}

void set_var_monocoag_pwr_str(const char *value) {}

const char *get_var_monocoag_mode_str() {

}

void set_var_monocoag_mode_str(const char *value) {}

int32_t get_var_led_pwr_active_br() {

}

void set_var_led_pwr_active_br(int32_t value) {}

int32_t get_var_led_pwr_active_color() {

}

void set_var_led_pwr_active_color(int32_t value) {}

int32_t get_var_led_ne_br() {

}

void set_var_led_ne_br(int32_t value) {}

const char *get_var_mono_bi_sel_str() {

}

void set_var_mono_bi_sel_str(const char *value) {}
