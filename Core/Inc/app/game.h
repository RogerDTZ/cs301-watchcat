/*
 * game.h
 *
 *  Created on: Dec 31, 2023
 *      Author: lipeiru
 */

#ifndef INC_APP_GAME_H_
#define INC_APP_GAME_H_
#include <stdio.h>
#include <lvgl.h>
#define MAX_BUTTONS 16

void open_app_game();
void close_app_game();
void dfs(uint8_t index, uint8_t value);
extern uint8_t img_index_array[16];
extern bool linked[MAX_BUTTONS];

extern bool visited[MAX_BUTTONS];
#endif /* INC_APP_GAME_H_ */
