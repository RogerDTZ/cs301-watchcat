#include "app/calc.h"

#include "sl_ui/ui.h"

#include "lvgl/lvgl.h"

void open_app_calc() { lv_obj_clear_flag(ui_CalcApp, LV_OBJ_FLAG_HIDDEN); }

void close_app_calc() { lv_obj_add_flag(ui_CalcApp, LV_OBJ_FLAG_HIDDEN); }
