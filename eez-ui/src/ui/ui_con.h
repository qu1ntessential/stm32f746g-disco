#ifndef UI_CON_H
#define UI_CON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "screens.h"
#include "uart_log.h"

/**
 * @struct UIEvent
 * @brief —труктура дл€ передачи событий в задачу обработки логики прибора
 */
typedef struct {
    lv_event_code_t event_code; /**< “ип событи€ */
    uint32_t widget_id;
    union {
        int32_t int_val;
        float float_val;
        bool bool_val;
        char *str_val;
    } data;
    uint32_t timestamp;
} UIEvent_t;

typedef enum {
    UI_CMD_UPDATE_LABEL,
    UI_CMD_CHANGE_SCREEN,
    UI_CMD_TOGGLE_LED
} UICmdType_t;

typedef struct {
    UICmdType_t cmd_type;
    uint32_t widget_id;
    union {
        int32_t int_val;
        char text[50];
    } data;
} UICmd_t;

typedef enum {
    // Buttons
    ID_BTN_MONO_BI_SEL = 1001,
    ID_BTN_CUT_MIX_PWR_INC,
    ID_BTN_CUT_MIX_PWR_DEC,
    ID_BTN_CUT_MODE_SEL,
    ID_BTN_MIX_MODE_SEL,
    ID_BTN_MONOCOAG_PWR_INC,
    ID_BTN_MONOCOAG_PWR_DEC,
    ID_BTN_MONOCOAG_MODE_SEL,
    ID_BTN_BICOAG_PWR_INC,
    ID_BTN_BICOAG_PWR_DEC,
    ID_BTN_BICOAG_MODE_SEL,

    // Leds
    ID_LED_PWR_ACTIVE = 2001,
    ID_LED_NE,

    // Labels
    ID_LABEL_CUT_MIX_PWR = 3001,
    ID_LABEL_MONOCOAG_PWR,
    ID_LABEL_BICOAG_PWR,
    ID_LABEL_MONO_BI_SEL,
    ID_LABEL_CUT_MIX_PWR_INC,
    ID_LABEL_CUT_MIX_PWR_DEC,
    ID_LABEL_CUT_MODE_SEL,
    ID_LABEL_MIX_MODE_SEL,
    ID_LABEL_MONOCOAG_PWR_INC,
    ID_LABEL_MONOCOAG_PWR_DEC,
    ID_LABEL_MONOCOAG_MODE_SEL,
    ID_LABEL_BICOAG_PWR_INC,
    ID_LABEL_BICOAG_PWR_DEC,
    ID_LABEL_BICOAG_MODE_SEL,

    ID_INVALID = 0xFFFF
} WidgetID;

void setWidgetsUserData(objects_t *objs);

#ifdef __cplusplus
}
#endif

#endif //UI_CON_H
