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
                               Variables
 =============================================================================*/
// 图片数组
const lv_img_dsc_t * imgset_egg_level[9] = {
		&ui_img_egg_level0_png,
		&ui_img_egg_level1_png, &ui_img_egg_level2_png, &ui_img_egg_level3_png, &ui_img_egg_level4_png,
		&ui_img_egg_level5_png, &ui_img_egg_level6_png, &ui_img_egg_level7_png, &ui_img_egg_level9_png};
lv_obj_t* GameButtons[16] = {};
int score = 0;
int level = 1;
uint8_t selected = 17; // Indicate which button is selected
uint8_t new_img_index = 6; // 即将加入的图片的index
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
void setButton(uint8_t index, uint8_t img_index){
	/**
	 * 设置button的背景图片为img_index对应的图片
	 */
	lv_obj_set_style_bg_color(GameButtons[index], lv_color_hex(0xFFE4C4), LV_PART_MAIN | LV_STATE_DEFAULT ); //设置背景颜色
	lv_obj_set_style_bg_img_src( GameButtons[index], imgset_egg_level[img_index], LV_PART_MAIN | LV_STATE_DEFAULT ); //设置背景图片
	lv_obj_set_style_border_width(GameButtons[index], 1, LV_PART_MAIN| LV_STATE_DEFAULT); //设置边框宽度
}
void resetButton(uint8_t index){
	/**
	 * 将button的背景图片换成初始图
	 */
	lv_obj_set_style_bg_color(GameButtons[index], lv_color_hex(0xFFE4C4), LV_PART_MAIN | LV_STATE_DEFAULT ); //设置背景颜色
	lv_obj_set_style_bg_img_src( GameButtons[index], imgset_egg_level[0], LV_PART_MAIN | LV_STATE_DEFAULT ); //设置背景图片
	lv_obj_set_style_border_width(GameButtons[index], 1, LV_PART_MAIN| LV_STATE_DEFAULT); //设置边框宽度
	img_index_array[index] = 0;
}
void selectButton(uint8_t index, uint8_t img_index){
	/**
	 * 选中button
	 */
	// 改变背景颜色
	lv_obj_set_style_bg_color(GameButtons[index], lv_color_hex(0xDFC4A4), LV_PART_MAIN | LV_STATE_DEFAULT );
	// 改变边框宽度
	lv_obj_set_style_border_width(GameButtons[index], 3, LV_PART_MAIN| LV_STATE_DEFAULT);
	// 改变背景图片
	lv_obj_set_style_bg_img_src( GameButtons[index], imgset_egg_level[img_index], LV_PART_MAIN | LV_STATE_DEFAULT ); //设置背景图片
}
void unselectButton(uint8_t index){
	/**
	 * 取消选中button
	 */
	// 还原背景颜色
	lv_obj_set_style_bg_color(GameButtons[index], lv_color_hex(0xFFE4C4), LV_PART_MAIN | LV_STATE_DEFAULT );
	// 还原边框宽度
	lv_obj_set_style_border_width(GameButtons[index], 1, LV_PART_MAIN| LV_STATE_DEFAULT);
	// 还原背景图片
	lv_obj_set_style_bg_img_src( GameButtons[index], &ui_img_egg_level0_png, LV_PART_MAIN | LV_STATE_DEFAULT ); //设置背景图片
}





// 检查坐标是否有效
bool isValidIndex(uint8_t index) {
    return index >= 0 && index < 16;
}

uint8_t img_index_array[] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
bool linked[MAX_BUTTONS];
bool visited[MAX_BUTTONS];

// DFS 函数
void dfs(uint8_t index, uint8_t value) {
    // 标记当前按钮已访问
    visited[index] = true;

    // 这里可以添加你对按钮的合并操作，例如合并按钮的逻辑
    if(img_index_array[index] == value){
    	linked[index] = true;
    }else{
    	return;
    }

    // 定义四个方向的相邻坐标
    uint8_t direction[4] = {1, 4, -1, -4};

    // 遍历四个方向
    for (int i = 0; i < 4; ++i) {
        uint8_t newIndex = index + direction[i];
        // 检查相邻坐标是否有效且未访问
        if (isValidIndex(newIndex) && !visited[newIndex]) {
            dfs(newIndex, value);
        }
    }
}
void confirmButton(uint8_t index, uint8_t new_img_index){
	/**
	 * 确定选中button
	 */
	//TODO: 遍历index对应的button周围的按钮，找到所有直接或者间接与index对应的button相邻的buttons进行合并。
	// 初始化访问状态数组
	int score_to_add = 0;
	img_index_array[index] = new_img_index-1;
	while(true){
		img_index_array[index]++;

	    for (int i = 0; i < MAX_BUTTONS; ++i) {
	        visited[i] = false;
	        linked[i] = false;
	    }
	    // 当前index的图片编号
	    uint8_t value = img_index_array[index];
	    // 调用深度优先搜索
	    dfs(index, (uint8_t)value);

	    int count = 0;
	    for (int i = 0; i < MAX_BUTTONS; ++i) {
	    	if(linked[i]){
	    		count++;
	    	}
	    }

//	     Convert count to a string


	    if(count >= 3){ //如果可以合并
	    	//清除相邻的图片
	    	for (int i = 0; i < MAX_BUTTONS; ++i) {
	    		if(linked[i] && i != index){
	    		    resetButton(i);
	    		}
	    	}
//	    	img_index_array[index]++;
	    	score_to_add += count * value;
	    }else{
	    	break;
	    }
	}

	    // 还原背景颜色
	    lv_obj_set_style_bg_color(GameButtons[index], lv_color_hex(0xFFE4C4), LV_PART_MAIN | LV_STATE_DEFAULT );
	    // 还原边框宽度
	    lv_obj_set_style_border_width(GameButtons[index], 1, LV_PART_MAIN| LV_STATE_DEFAULT);
	    //设置背景图片
	    setButton(index, img_index_array[index]);
	    //清除选中状态
	    selected = 17;
	    // 加分
	    score += score_to_add;
	    char scoreStr[10]; // Assuming a reasonable size for the string
	    sprintf(scoreStr, "Score: %d ", score);
	    lv_label_set_text(ui_LabelScore, scoreStr);
	    if(img_index_array[index]==8){
	    	lv_obj_clear_flag(ui_CongratulationPanel, LV_OBJ_FLAG_HIDDEN);
	    }
	    // Debug
	    	    // Convert count to a string
//	    	        char countStr[10]; // Assuming a reasonable size for the string
//	    	        sprintf(countStr, "%d %d %d %d", img_index_array[0], img_index_array[1], img_index_array[2], img_index_array[3]);
//	    	    lv_label_set_text(ui_LabelScore, countStr);

}
void clickHandler(uint8_t index){
	//	lv_label_set_text(ui_LabelScore, "Game Button index");
	if(img_index_array[index] == 0){ // 只有点击空位置时才产生反应
		if(selected == index){ // 双击确认
			confirmButton(selected, new_img_index);
		}else{
			if(selected < 17){ // 当前有选中的对象
				unselectButton(selected);
			}
			selected = index;
			selectButton(selected, new_img_index);
		}
	}
}
/*=============================================================================
                             UI event handlers
 =============================================================================*/

void GameButton1Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 1");
	clickHandler(0);
};
void GameButton2Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 2");
	clickHandler(1);
};
void GameButton3Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 3");
	clickHandler(2);
};
void GameButton4Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 4");
	clickHandler(3);
};
void GameButton5Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 5");
	clickHandler(4);
};
void GameButton6Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 6");
	clickHandler(5);
};
void GameButton7Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 7");
	clickHandler(6);
};
void GameButton8Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 8");
	clickHandler(7);
};
void GameButton9_Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 9");
	clickHandler(8);
};
void GameButton10Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 10");
	clickHandler(9);
};
void GameButton11Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 11");
	clickHandler(10);
};
void GameButton12Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 12");
	clickHandler(11);
};
void GameButton13Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 13");
	clickHandler(12);
};
void GameButton14Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 14");
	clickHandler(13);
};
void GameButton15Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 15");
	clickHandler(14);
};
void GameButton16Clicked( lv_event_t * e){
//	lv_label_set_text(ui_LabelScore, "Game Button 16");
	clickHandler(15);
};
void ButtonLevelClicked(lv_event_t * e){
	level = (level+1)%7;
	if(level == 0){
		level = 1;
	}
	char levelStr[20];
	sprintf(levelStr, "Level: %d", level);
	lv_label_set_text(ui_LabelLevel, levelStr);

	new_img_index = 7 - level;
}
void Game_Restart(lv_event_t * e){
	score = 0;
	char scoreStr[10]; // Assuming a reasonable size for the string
    sprintf(scoreStr, "Score: %d ", score);
    lv_label_set_text(ui_LabelScore, scoreStr);

	lv_obj_add_flag(ui_CongratulationPanel, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(ui_IntroductionPanel, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(ui_LabelRestart, "Restart");
	for(int i = 0;i<16;++i){
		if(rand()%10<2){
			img_index_array[i] = rand() % new_img_index+1;
		}else{
			img_index_array[i] = 0;
		}
	}
	initGameButtons(GameButtons);
	//TODO: change the background image according to 'img_index_array'
	for(int i = 0; i < 16; i++){
		setButton(i, img_index_array[i]);
	}
//	setButton(0);
//	clickHandler(15);
};
