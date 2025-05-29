#include "actions.h"
#include "images.h"

#include "OsTasks.h"

extern FatFsWrapper uSD;
extern uint32_t my_fb[TFT_HOR_RES * TFT_VER_RES];

extern void action_test_sd(lv_event_t *e) {
    lv_obj_clean(lv_scr_act());

    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "A:/1.bmp");
    lv_obj_set_pos(img, 0, 0);

    lv_obj_invalidate(lv_scr_act());
    lv_timer_handler();
}

extern void action_test_flash(lv_event_t *e) {
    static lv_obj_t *img;

    if (!img) {
        img = lv_img_create(lv_scr_act());  // Создаем на активном экране
        lv_img_set_src(img, &img_lion); // Устанавливаем массив как источник
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0); // Центрируем
    } else {
        lv_obj_del(img);  // Удаляем изображение при повторном нажатии
        img = NULL;
    }
}