#include "vars.h"
#include "stdio.h"
#include "config.h"

static char str[20];

static esgParam_t param = {
        .mono_bi_sel = true,
        .cut_mix_sel = true,
        .mono_bi_coag_sel = true,
        .monocut_0 = 0,
        .monocut_1 = 0,
        .monocut_2 = 0,
        .monocut_mode = 0,
        .monomix_0 = 0,
        .monomix_1 = 0,
        .monomix_2 = 0,
        .monomix_mode = 0,
        .monocoag_0 = 0,
        .monocoag_1 = 0,
        .monocoag_2 = 0,
        .monocoag_mode = 0,
        .bicut_0 = 0,
        .bicut_1 = 0,
        .bicut_2 = 0,
        .bicut_mode = 0,
        .bimix_0 = 0,
        .bimix_1 = 0,
        .bimix_2 = 0,
        .bimix_mode = 0,
        .bicoag_0 = 0,
        .bicoag_1 = 0,
        .bicoag_2 = 0,
        .bicoag_mode = 0
};

/**
 * @brief Функция инкрементирования или декрементирования мощностей режимов
 * @param select 0b000 - monocut,
 *               0b001 - bicut,
 *               0b010 - monomix
 *               0b011 - bimix
 *               0b100 - monocoag,
 *               0b101 - bicoag
 * @param regime Вариант режима (0, 1 или 2)
 * @param step   Шаг увеличения переменной
 * @param isInc  true - инкрементация, false - декрементация
 * @return
 */
bool changePwr(uint8_t select, uint8_t regime, uint8_t step, bool isInc) {
    if (select > 5 || regime > 2) return false;

    // Макрос для безопасного изменения значений
#define ADJUST_VALUE(field, max, min) \
        do { \
            uint16_t new_val; \
            if (isInc) { \
                new_val = (param.field + step > (max)) ? (max) : param.field + step; \
            } else { \
                new_val = (param.field < (min) + step) ? (min) : param.field - step; \
            } \
            param.field = new_val; \
        } while(0)

    switch (select) {
        case 0: // monocut
            switch (regime) {
                case 0: ADJUST_VALUE(monocut_0, MONOCUT0_MAX_PWR, MONOCUT0_MIN_PWR); break;
                case 1: ADJUST_VALUE(monocut_1, MONOCUT1_MAX_PWR, MONOCUT1_MIN_PWR); break;
                case 2: ADJUST_VALUE(monocut_2, MONOCUT2_MAX_PWR, MONOCUT2_MIN_PWR); break;
                default: return false;
            }
            break;

        case 1: // bicut
            switch (regime) {
                case 0: ADJUST_VALUE(bicut_0, BICUT0_MAX_PWR, BICUT0_MIN_PWR); break;
                case 1: ADJUST_VALUE(bicut_1, BICUT1_MAX_PWR, BICUT1_MIN_PWR); break;
                case 2: ADJUST_VALUE(bicut_2, BICUT2_MAX_PWR, BICUT2_MIN_PWR); break;
                default: return false;
            }
            break;

        case 2: // monomix
            switch (regime) {
                case 0: ADJUST_VALUE(monomix_0, MONOMIX0_MAX_PWR, MONOMIX0_MIN_PWR); break;
                case 1: ADJUST_VALUE(monomix_1, MONOMIX1_MAX_PWR, MONOMIX1_MIN_PWR); break;
                case 2: ADJUST_VALUE(monomix_2, MONOMIX2_MAX_PWR, MONOMIX2_MIN_PWR); break;
                default: return false;
            }
            break;

        case 3: // bimix
            switch (regime) {
                case 0: ADJUST_VALUE(bimix_0, BIMIX0_MAX_PWR, BIMIX0_MIN_PWR); break;
                case 1: ADJUST_VALUE(bimix_1, BIMIX1_MAX_PWR, BIMIX1_MIN_PWR); break;
                case 2: ADJUST_VALUE(bimix_2, BIMIX2_MAX_PWR, BIMIX2_MIN_PWR); break;
                default: return false;
            }
            break;

        case 4: // monocoag
            switch (regime) {
                case 0: ADJUST_VALUE(monocoag_0, MONOCOAG0_MAX_PWR, MONOCOAG0_MIN_PWR); break;
                case 1: ADJUST_VALUE(monocoag_1, MONOCOAG1_MAX_PWR, MONOCOAG1_MIN_PWR); break;
                case 2: ADJUST_VALUE(monocoag_2, MONOCOAG2_MAX_PWR, MONOCOAG2_MIN_PWR); break;
                default: return false;
            }
            break;

        case 5: // bicoag
            switch (regime) {
                case 0: ADJUST_VALUE(bicoag_0, BICOAG0_MAX_PWR, BICOAG0_MIN_PWR); break;
                case 1: ADJUST_VALUE(bicoag_1, BICOAG1_MAX_PWR, BICOAG1_MIN_PWR); break;
                case 2: ADJUST_VALUE(bicoag_2, BICOAG2_MAX_PWR, BICOAG2_MIN_PWR); break;
                default: return false;
            }
            break;

        default: return false;
    }

#undef ADJUST_VALUE
    return true;
}

/**
 * @brief Функция инкремента режима
 * @param select 0b00 - cut,
 *               0b01 - mix,
 *               0b10 - coag
 */
bool changeMode(uint8_t select) {
    if (select > 2) return false;

    if (!param.mono_bi_sel) { // bipolar select
        switch (select) {
            case 0: // bicut
                if (param.cut_mix_sel) {
                    if (++param.bicut_mode > 2)
                        param.bicut_mode = 0;
                } else {
                    param.cut_mix_sel = !param.cut_mix_sel;
                }
                break;
            case 1: // bimix
                if (!param.cut_mix_sel) {
                    if (++param.bimix_mode > 2)
                        param.bimix_mode = 0;
                } else {
                    param.cut_mix_sel = !param.cut_mix_sel;
                }
                break;
            case 2: // bicoag
                if (!param.mono_bi_coag_sel) {
                    if (++param.bicoag_mode > 2)
                        param.bicoag_mode = 0;
                } else {
                    param.mono_bi_coag_sel = !param.mono_bi_coag_sel;
                }
                break;
            default:
                return false;
        }
    } else { // monopolar select
        switch (select) {
            case 0: // monocut
                if (param.cut_mix_sel) {
                    if (++param.monocut_mode > 2)
                        param.monocut_mode = 0;
                } else {
                    param.cut_mix_sel = !param.cut_mix_sel;
                }
                break;
            case 1: // monomix
                if (!param.cut_mix_sel) {
                    if (++param.monomix_mode > 2)
                        param.monomix_mode = 0;
                } else {
                    param.cut_mix_sel = !param.cut_mix_sel;
                }
                break;
            case 2: // monocoag
                if (param.mono_bi_coag_sel) {
                    if (++param.monocoag_mode > 2)
                        param.monocoag_mode = 0;
                } else {
                    param.mono_bi_coag_sel = !param.mono_bi_coag_sel;
                }
                break;
            default:
                return false;
        }
    }
    return true;
}

void changeMonoBiSel(void) {
    param.mono_bi_sel = !param.mono_bi_sel;
    param.mono_bi_coag_sel = 1;
}

/**
 *
 * @param sel_flag В переменную считывается значение поля param.cut_mix_sel
 * @param mode_flag В переменную считывается значение поля
 *                  param.cut_mode или param.mix_mode  зависимости от param.cut_mix_sel
 */
void getCutMixSel(const bool *sel_flag, const uint8_t *mode_flag) {
    if (sel_flag)
        *(bool *) sel_flag = param.cut_mix_sel;
    if (param.cut_mix_sel) {
        if (mode_flag)
            *(uint8_t *) mode_flag = param.monocut_mode;
    } else {
        if (mode_flag)
            *(uint8_t *) mode_flag = param.monomix_mode;
    }
}

/**
 *
 * @param sel_flag В переменную считывается значение поля param.mono_bi_coag_sel
 * @param mode_flag В переменную считывается значение поля
 *                  param.monocoag_mode или param.bicoag_mode  зависимости от param.mono_bi_coag_sel
 */
void getMonoBiCoagSel(const bool *sel_flag, const uint8_t *mode_flag) {
    if (sel_flag)
        *(bool *) sel_flag = param.mono_bi_coag_sel;
    if (param.mono_bi_coag_sel) {
        if (mode_flag)
            *(uint8_t *) mode_flag = param.monocoag_mode;
    } else {
        if (mode_flag)
            *(uint8_t *) mode_flag = param.bicoag_mode;
    }
}

const char *get_var_cut_mix_pwr_str(void) {
    if (param.cut_mix_sel) {
        switch (param.monocut_mode) {
            case 0:
                sprintf(str, "%hu", param.monocut_0);
                break;
            case 1:
                sprintf(str, "%hu", param.monocut_1);
                break;
            case 2:
                sprintf(str, "%hu", param.monocut_2);
                break;
            default:
                break;
        }
    } else {
        switch (param.monomix_mode) {
            case 0:
                sprintf(str, "%hu", param.monomix_0);
                break;
            case 1:
                sprintf(str, "%hu", param.monomix_1);
                break;
            case 2:
                sprintf(str, "%hu", param.monomix_2);
                break;
            default:
                break;
        }
    }
    return str;
}

void set_var_cut_mix_pwr_str(const char *value) {}

const char *get_var_cut_mode_str(void) {
    sprintf(str, "%d", param.monocut_mode);
    return str;
}

void set_var_cut_mode_str(const char *value) {}

const char *get_var_bicoag_pwr_str(void) {
    switch (param.bicoag_mode) {
        case 0:
            sprintf(str, "%hu", param.bicoag_0);
            break;
        case 1:
            sprintf(str, "%hu", param.bicoag_1);
            break;
        case 2:
            sprintf(str, "%hu", param.bicoag_2);
            break;
        default:
            break;
    }
    return str;
}

void set_var_bicoag_pwr_str(const char *value) {}

const char *get_var_bicoag_mode_str(void) {
    sprintf(str, "%d", param.bicoag_mode);
    return str;
}

void set_var_bicoag_mode_str(const char *value) {}

const char *get_var_mix_mode_str(void) {
    sprintf(str, "%d", param.monomix_mode);
    return str;
}

void set_var_mix_mode_str(const char *value) {}

const char *get_var_monocoag_pwr_str(void) {
    switch (param.monocoag_mode) {
        case 0:
            sprintf(str, "%hu", param.monocoag_0);
            break;
        case 1:
            sprintf(str, "%hu", param.monocoag_1);
            break;
        case 2:
            sprintf(str, "%hu", param.monocoag_2);
            break;
        default:
            break;
    }
    return str;
}

void set_var_monocoag_pwr_str(const char *value) {}

const char *get_var_monocoag_mode_str(void) {
    sprintf(str, "%d", param.monocoag_mode);
    return str;
}

void set_var_monocoag_mode_str(const char *value) {}

int32_t get_var_led_pwr_active_br(void) {
    return param.led_pwr_active_br;
}

void set_var_led_pwr_active_br(int32_t value) {}

int32_t get_var_led_pwr_active_color(void) {
    return param.led_pwr_active_color;
}

void set_var_led_pwr_active_color(int32_t value) {}

int32_t get_var_led_ne_br(void) {
    return param.led_ne_br;
}

void set_var_led_ne_br(int32_t value) {}

const char *get_var_mono_bi_sel_str(void) {
    sprintf(str, "%d", param.mono_bi_sel);
    return str;
}

void set_var_mono_bi_sel_str(const char *value) {}
