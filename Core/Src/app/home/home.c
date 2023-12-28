#include "app/home.h"

#include "lvgl.h"
#include "sl_ui/ui.h"

enum open_state curr_open;

void enter_home() { lv_obj_clear_flag(ui_Home, LV_OBJ_FLAG_HIDDEN); }

void exit_home() { lv_obj_add_flag(ui_Home, LV_OBJ_FLAG_HIDDEN); }
