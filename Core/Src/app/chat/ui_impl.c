#include <assert.h>
#include <stdio.h>

#include "lvgl.h"
#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

#include "app/chat.h"
#include "radio/radio.h"

static radio_uid_t get_chatter(int id)
{
  radio_uid_t local_uid = get_uid();
  assert(0 <= local_uid && local_uid < USER_NUM);

  if (local_uid == 0) {
    return id == 1 ? 1 : 2;
  } else if (local_uid == 1) {
    return id == 1 ? 0 : 2;
  } else {
    return id == 1 ? 0 : 1;
  }
}

static radio_uid_t get_session(radio_uid_t uid)
{
  radio_uid_t local_uid = get_uid();
  assert(0 <= local_uid && local_uid < USER_NUM);

  if ((local_uid == 0 && uid == 1) || (local_uid == 1 && uid == 0)) {
    return SESSION_ID_0_1;
  } else if ((local_uid == 0 && uid == 2) || (local_uid == 2 && uid == 0)) {
    return SESSION_ID_0_2;
  } else {
    assert((local_uid == 1 && uid == 2) || (local_uid == 2 && uid == 1));
    return SESSION_ID_1_2;
  }
}

void ChatChat1Clicked(lv_event_t *e)
{
  radio_uid_t chatter = get_chatter(1);
  action_invite(chatter, get_session(chatter));
}

void ChatChat2Clicked(lv_event_t *e)
{
  radio_uid_t chatter = get_chatter(2);
  action_invite(chatter, get_session(chatter));
}

void ChatGroupClicked(lv_event_t *e) {}
