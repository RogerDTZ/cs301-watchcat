#include <assert.h>
#include <stdio.h>

#include "lvgl.h"
#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

#include "app/chat.h"
#include "radio/radio.h"

radio_session_t get_session_with(radio_uid_t uid)
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

radio_uid_t get_chatter_from_session(radio_session_t session)
{
  assert(session != SESSION_ID_0_1_2 && "Should not pass in group session");

  radio_uid_t local_uid = get_uid();
  assert(0 <= local_uid && local_uid < USER_NUM);

  if (local_uid == 0) {
    return session == SESSION_ID_0_1 ? 1 : 2;
  } else if (local_uid == 1) {
    return session == SESSION_ID_0_1 ? 0 : 2;
  } else {
    return session == SESSION_ID_0_2 ? 0 : 1;
  }
}

/**
 * @brief Return the uid of the chatter in the UI
 *
 * @param id 1 or 2, indicating the first or second single-chat
 */
static radio_uid_t get_single_chatter_uid(int ui_id)
{
  assert((ui_id == 1 || ui_id == 2) && "Invalid ui_id");
  radio_uid_t local_uid = get_uid();
  assert(0 <= local_uid && local_uid < USER_NUM);

  if (local_uid == 0) {
    return ui_id == 1 ? 1 : 2;
  } else if (local_uid == 1) {
    return ui_id == 1 ? 0 : 2;
  } else {
    return ui_id == 1 ? 0 : 1;
  }
}

void event_user_online(radio_uid_t uid)
{
  lv_label_set_text_fmt(ui_Label4, "User %d is online", uid);
}

void event_user_offline(radio_uid_t uid)
{
  lv_label_set_text_fmt(ui_Label4, "User %d is offline", uid);
}

void ChatChat1Clicked(lv_event_t *e)
{
  radio_uid_t chatter = get_single_chatter_uid(1);
  action_message(get_session_with(chatter), "Hello! You are my chat 1");
}

void ChatChat2Clicked(lv_event_t *e)
{
  radio_uid_t chatter = get_single_chatter_uid(2);
  action_message(get_session_with(chatter), "Hello! You are my chat 2");
}

void ChatGroupClicked(lv_event_t *e)
{
  action_message(SESSION_ID_0_1_2, "Hello! Speaking in group");
}
