#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "lvgl.h"

void touch_indev_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);

void init_lvgl_input_devices();

#endif /* __TOUCH_H__*/
