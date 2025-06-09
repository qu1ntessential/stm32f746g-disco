#include "actions.h"
#include "images.h"

#include "OsTasks.h"

extern FatFsWrapper uSD;

extern void action_test_sd(lv_event_t *e) {
    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "A:/1.bmp");
    lv_obj_set_pos(img, 0, 0);
}

extern void action_test_flash(lv_event_t *e) {

}