#include "actions.h"

void action_button_action(lv_event_t *e) {
    lv_obj_t *img = lv_img_create(lv_scr_act()); ///< Создаём объект изображения
    lv_img_set_src(img, "A:/1.bmp");            ///< Путь к файлу на SD-карте
    lv_refr_now(NULL);
    lv_obj_set_pos(img, 0, 0);                ///< Устанавливаем координаты
}
