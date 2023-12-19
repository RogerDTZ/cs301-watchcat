#include "port/input_dev.h"

static lv_indev_drv_t touch_indev_drv;

static lv_indev_t *touch_indev;

static void init_touch_input()
{
  touch_indev_drv.type = LV_INDEV_TYPE_POINTER;
  touch_indev_drv.read_cb = touch_indev_read_cb;
  touch_indev = lv_indev_drv_register(&touch_indev_drv);
}

void init_lvgl_input_devices()
{
  init_touch_input();
  // init other input devices, e.g. keyboard?
}
