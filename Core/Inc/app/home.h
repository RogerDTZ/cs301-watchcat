#ifndef __HOME_H__
#define __HOME_H__

enum open_state {
  OPEN_STATE_HOME,
  OPEN_STATE_CHAT,
  OPEN_STATE_CALC,
  OPEN_STATE_ALBUM,
};

void enter_home();

void exit_home();

void update_time();

#endif /* __HOME_H__ */
