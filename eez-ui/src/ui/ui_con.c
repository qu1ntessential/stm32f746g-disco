#include "ui_con.h"

typedef struct {
    lv_obj_t **widget_ptr;
    uint32_t widget_id;
} WidgetMapEntry_t;

void setWidgetsUserData(objects_t *objs) {
    if (objs == NULL) {
        print_log(ERROR_LOG, "setWidgetsUserData(): NULL objects pointer\r\n");
        return;
    }

    const WidgetMapEntry_t WIDGET_MAP[] = {
            // Buttons
            {&objs->btn_mono_bi_sel,       ID_BTN_MONO_BI_SEL},
            {&objs->btn_cut_mix_pwr_inc,   ID_BTN_CUT_MIX_PWR_INC},
            {&objs->btn_cut_mix_pwr_dec,   ID_BTN_CUT_MIX_PWR_DEC},
            {&objs->btn_cut_mode_sel,      ID_BTN_CUT_MODE_SEL},
            {&objs->btn_mix_mode_sel,      ID_BTN_MIX_MODE_SEL},
            {&objs->btn_monocoag_pwr_inc,  ID_BTN_MONOCOAG_PWR_INC},
            {&objs->btn_monocoag_pwr_dec,  ID_BTN_MONOCOAG_PWR_DEC},
            {&objs->btn_monocoag_mode_sel, ID_BTN_MONOCOAG_MODE_SEL},
            {&objs->btn_bicoag_pwr_inc,    ID_BTN_BICOAG_PWR_INC},
            {&objs->btn_bicoag_pwr_dec,    ID_BTN_BICOAG_PWR_DEC},
            {&objs->btn_bicoag_mode_sel,   ID_BTN_BICOAG_MODE_SEL},

            // Labels
            {&objs->label_mono_bi_sel,     ID_LABEL_MONO_BI_SEL},
            {&objs->label_cut_mix_pwr,     ID_LABEL_CUT_MIX_PWR},

            // Terminator entry
            {NULL,                         0}
    };

    for (const WidgetMapEntry_t *entry = WIDGET_MAP; entry->widget_ptr != NULL; ++entry) {
        if (*entry->widget_ptr != NULL) {
            lv_obj_set_user_data(*entry->widget_ptr, (void *) entry->widget_id);
        }
    }
}
