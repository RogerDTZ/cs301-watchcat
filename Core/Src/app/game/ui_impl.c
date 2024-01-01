/*
 * ui_impl.c
 *
 *  Created on: Dec 31, 2023
 *      Author: lipeiru
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lvgl.h"
#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

#include "app/game.h"

/*=============================================================================
                               UI controllers
 =============================================================================*/
void initGameButtons(lv_obj_t **buttons) {
    buttons[0] = ui_GameButton1;
    buttons[1] = ui_GameButton2;
    buttons[2] = ui_GameButton3;
    buttons[3] = ui_GameButton4;
    buttons[4] = ui_GameButton5;
    buttons[5] = ui_GameButton6;
    buttons[6] = ui_GameButton7;
    buttons[7] = ui_GameButton8;
    buttons[8] = ui_GameButton9;
    buttons[9] = ui_GameButton10;
    buttons[10] = ui_GameButton11;
    buttons[11] = ui_GameButton12;
    buttons[12] = ui_GameButton13;
    buttons[13] = ui_GameButton14;
    buttons[14] = ui_GameButton15;
    buttons[15] = ui_GameButton16;
}
/*=============================================================================
                             UI event handlers
 =============================================================================*/
// 图片数组
const lv_img_dsc_t * imgset_egg_level[8] = {
		&ui_img_egg_level1_png, &ui_img_egg_level2_png, &ui_img_egg_level3_png, &ui_img_egg_level4_png,
		&ui_img_egg_level5_png, &ui_img_egg_level6_png, &ui_img_egg_level7_png, &ui_img_egg_level9_png};
lv_obj_t* GameButtons[16] = {};
uint8_t img_index_array[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t selected = 0; // Indicate which button is selected
void GameButton1Clicked( lv_event_t * e){};
void GameButton2Clicked( lv_event_t * e){};
void GameButton3Clicked( lv_event_t * e){};
void GameButton4Clicked( lv_event_t * e){};
void GameButton5Clicked( lv_event_t * e){};
void GameButton6Clicked( lv_event_t * e){};
void GameButton7Clicked( lv_event_t * e){};
void GameButton8Clicked( lv_event_t * e){};
void GameButton9_Clicked( lv_event_t * e){};
void GameButton10Clicked( lv_event_t * e){};
void GameButton11Clicked( lv_event_t * e){};
void GameButton12Clicked( lv_event_t * e){};
void GameButton13Clicked( lv_event_t * e){};
void GameButton14Clicked( lv_event_t * e){};
void GameButton15Clicked( lv_event_t * e){};
void GameButton16Clicked( lv_event_t * e){};
void Game_Restart(lv_event_t * e){
	initGameButtons(GameButtons);
	//TODO: change the background image according to 'img_index_array'
	for(int i = 0; i < 16; i++){
		lv_obj_set_style_bg_img_src( GameButtons[i], imgset_egg_level[img_index_array[i]], LV_PART_MAIN | LV_STATE_DEFAULT ); //设置背景图片
	}
};
