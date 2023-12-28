#include <assert.h>
#include <stdio.h>

#include "lvgl.h"
#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

#include "app/chat.h"
#include "radio/radio.h"

static void on_chat_switching()
{
  // [TODO] according to ui_curr_session, display content of the chat
  lv_label_set_text_fmt(ui_ChatDesc, "This is chat %d", (int)ui_curr_session);
}

static void set_chatter_online_ui(int ui_id, bool online)
{
  assert(1 <= ui_id && ui_id <= 2 && "Invalid ui_id");

  if (ui_id == 1) {
    if (online) {
      lv_obj_clear_flag(ui_Chatter1Online, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_Chatter1Offline, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_clear_flag(ui_Chatter1Offline, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_Chatter1Online, LV_OBJ_FLAG_HIDDEN);
    }
  } else {
    if (online) {
      lv_obj_clear_flag(ui_Chatter2Online, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_Chatter2Offline, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_clear_flag(ui_Chatter2Offline, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_Chatter2Online, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

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

radio_uid_t get_single_chatter_uid(int ui_id)
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
  assert(uid != get_uid() && "Should not be called for local user");

  const char *user_name = get_user_name(uid);
  // Show the cross-app notification
  lv_label_set_text_fmt(ui_Notification, "%s is online", user_name);
  // Toggle the online mark
  if (get_single_chatter_uid(1) == uid) {
    set_chatter_online_ui(1, true);
  } else if (get_single_chatter_uid(2) == uid) {
    set_chatter_online_ui(2, true);
  }
}

void event_user_offline(radio_uid_t uid)
{
  assert(uid != get_uid() && "Should not be called for local user");

  const char *user_name = get_user_name(uid);
  // Show the cross-app notification
  lv_label_set_text_fmt(ui_Notification, "%s went offline", user_name);
  // Toggle the online mark
  if (get_single_chatter_uid(1) == uid) {
    set_chatter_online_ui(1, false);
  } else if (get_single_chatter_uid(2) == uid) {
    set_chatter_online_ui(2, false);
  }
}

void ChatSelectedId0(lv_event_t *e) { event_selected_id(0); }
void ChatSelectedId1(lv_event_t *e) { event_selected_id(1); }
void ChatSelectedId2(lv_event_t *e) { event_selected_id(2); }

void ChatChat1Clicked(lv_event_t *e)
{
  radio_uid_t chatter = get_single_chatter_uid(1);

  // Clear the invitation mark
  lv_obj_add_flag(ui_Chatter1Invite, LV_OBJ_FLAG_HIDDEN);

  if (ui_curr_session != UI_SESSION_CHAT1) {
    ui_curr_session = UI_SESSION_CHAT1;
    on_chat_switching();
  } else {
    // re-click issues an invitation
    action_invite(chatter, get_session_with(chatter));
  }
}

void ChatChat2Clicked(lv_event_t *e)
{
  radio_uid_t chatter = get_single_chatter_uid(2);

  // Clear the invitation mark
  lv_obj_add_flag(ui_Chatter2Invite, LV_OBJ_FLAG_HIDDEN);

  if (ui_curr_session != UI_SESSION_CHAT2) {
    ui_curr_session = UI_SESSION_CHAT2;
    on_chat_switching();
  } else {
    // re-click issues an invitation
    action_invite(chatter, get_session_with(chatter));
  }
}

void ChatGroupClicked(lv_event_t *e)
{
  // action_message(SESSION_ID_0_1_2, "Hello! Speaking in group");

  if (ui_curr_session != UI_SESSION_GROUP) {
    ui_curr_session = UI_SESSION_GROUP;
    on_chat_switching();
  }
}
