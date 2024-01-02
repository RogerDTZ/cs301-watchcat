#include "lvgl.h"
#include "sl_ui/ui.h"

#include "app/album.h"
#include "app/calc.h"
#include "app/chat.h"
#include "app/home.h"

extern enum open_state curr_open;

int time;

void HomeButonChatClicked(lv_event_t *e)
{
  assert(curr_open == OPEN_STATE_HOME);
  curr_open = OPEN_STATE_CHAT;
  exit_home();
  open_app_chat();
}
void HomeButonCalcClicked(lv_event_t *e)
{
  assert(curr_open == OPEN_STATE_HOME);
  curr_open = OPEN_STATE_CALC;
  exit_home();
  open_app_calc();
}
void HomeButonAlbumClicked(lv_event_t *e)
{
  assert(curr_open == OPEN_STATE_HOME);
  curr_open = OPEN_STATE_ALBUM;
  exit_home();
  open_app_album();
}

void HomeButtonClicked(lv_event_t *e)
{
  switch (curr_open) {
  case OPEN_STATE_HOME:
    break;
  case OPEN_STATE_CHAT:
    close_app_chat();
    enter_home();
    break;
  case OPEN_STATE_CALC:
    close_app_calc();
    enter_home();
    break;
  case OPEN_STATE_ALBUM:
    close_app_album();
    enter_home();
    break;
  default:
    assert(0 && "Invalid state");
  }
  curr_open = OPEN_STATE_HOME;
}

void update_time(){
  time++;
  if(time % 2 == 0){
    lv_label_set_text_fmt(ui_HomeTime, "Time since start: %d s", time / 2);
  }
}