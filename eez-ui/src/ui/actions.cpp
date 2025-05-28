#include "actions.h"

#include "OsTasks.h"

extern FatFsWrapper uSD;

extern void action_test(lv_event_t *e) {
    // 1. Создаем объект изображения
    lv_obj_t *img = lv_img_create(lv_scr_act());
    if (img == NULL) {
        print_log(ERROR_LOG, "Error: Failed to create image object\n");
        return;
    }

    // 2. Устанавливаем источник (без проверки возвращаемого значения)
    lv_img_set_src(img, "A:/4.bmp");

    // 3. Альтернативная проверка через свойство объекта
    if (lv_img_get_src(img) == NULL) {
        print_log(ERROR_LOG, "Error: Failed to load image\n");
        lv_obj_del(img);
        return;
    }

    // 4. Настраиваем позицию и обновляем экран
    lv_obj_set_pos(img, 0, 0);
    //lv_obj_move_background(img);
}
