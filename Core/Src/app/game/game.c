/*
 * game.c
 *
 *  Created on: Dec 31, 2023
 *      Author: lipeiru
 */
#include "app/game.h"

#include "sl_ui/ui.h"

#include "lvgl/lvgl.h"

void open_app_game() { lv_obj_clear_flag(ui_GameApp, LV_OBJ_FLAG_HIDDEN); }

void close_app_game() { lv_obj_add_flag(ui_GameApp, LV_OBJ_FLAG_HIDDEN); }


