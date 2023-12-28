#include "app/chat.h"

#include <stdio.h>
#include <string.h>

#include "main.h"

#include "sl_ui/ui.h"

#define USER_NAME_0 "Martha"
#define USER_NAME_1 "Tina"
#define USER_NAME_2 "Jackle"

enum ui_session ui_curr_session;

static int heartbeat_countdown;
static tick_t heartbeat_last_recv[USER_NUM];
static bool user_online[USER_NUM];
static int hb_counter[USER_NUM];

static void display_debug()
{
  lv_label_set_text_fmt(ui_DebugLabel, "[%s] %d %d %d",
                        get_user_name(get_uid()), hb_counter[0], hb_counter[1],
                        hb_counter[2]);
}

const char *get_user_name(radio_uid_t uid)
{
  assert(0 <= uid && uid < USER_NUM);

  switch (uid) {
  case 0:
    return USER_NAME_0;
  case 1:
    return USER_NAME_1;
  case 2:
    return USER_NAME_2;
  }
  return NULL;
}

void action_heartbeat()
{
  struct radio_prot_packet pkt;
  pkt.cmd = RADIO_PROT_CMD_HEARTBEAT;
  pkt.body.heartbeat.id = get_uid();

  radio_broadcast(&pkt);
}

void action_message(radio_session_t session, const char *msg)
{
  struct radio_prot_packet pkt;
  pkt.cmd = RADIO_PROT_CMD_MSG;
  pkt.body.msg.id = get_uid();
  pkt.body.msg.session = session;

  int msg_len = strlen(msg);
  assert(msg_len < MSG_MAX_LEN && "Message too long");
  memcpy(pkt.body.msg.msg, msg, msg_len);

  if (session == SESSION_ID_0_1_2) {
    radio_broadcast(&pkt);
  } else {
    radio_send(get_chatter_from_session(session), &pkt);
  }
}

bool action_invite(radio_uid_t uid, radio_session_t session)
{
  struct radio_prot_packet pkt;
  pkt.cmd = RADIO_PROT_CMD_INVITE;
  pkt.body.invite.id = get_uid();
  pkt.body.invite.session = session;

  return radio_send(uid, &pkt);
}

void radio_event_handler_heartbeat(struct radio_prot_heartbeat *heartbeat)
{
  radio_uid_t sender = heartbeat->id;

  hb_counter[sender] += 1;
  display_debug();

  heartbeat_last_recv[sender] = get_50hz_tick();
  if (!user_online[sender]) {
    event_user_online(sender);
  }
  user_online[sender] = true;
}

void radio_event_handler_message(struct radio_prot_msg *msg)
{
  radio_uid_t sender = msg->id;
  radio_session_t session = msg->session;

  lv_label_set_text_fmt(ui_ChatDesc, "(%d, %d): %s", session, sender, msg->msg);
}

void radio_event_handler_invite(struct radio_prot_invite *invite)
{
  radio_uid_t inviter = invite->id;
  radio_session_t session = invite->session;

  assert(session != SESSION_ID_0_1_2 && "Only single chatting can be invited");

  const char *inviter_name = get_user_name(inviter);
  // Show the cross-app notification
  lv_label_set_text_fmt(ui_Notification, "%s is inviting you", inviter_name);
  // Show the invitation mark
  if (get_single_chatter_uid(1) == inviter) {
    if (ui_curr_session != UI_SESSION_CHAT1) {
      lv_obj_clear_flag(ui_Chatter1Invite, LV_OBJ_FLAG_HIDDEN);
    }
  } else {
    if (ui_curr_session != UI_SESSION_CHAT2) {
      lv_obj_clear_flag(ui_Chatter2Invite, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

void chat_init(radio_uid_t uid)
{
  radio_init(uid);

  heartbeat_countdown = 0;
  memset(heartbeat_last_recv, 0, sizeof(heartbeat_last_recv));
  memset(user_online, 0, sizeof(user_online));

  ui_curr_session = UI_SESSION_NONE;

  lv_label_set_text(ui_Chatter1Name, get_user_name(get_single_chatter_uid(1)));
  lv_label_set_text(ui_Chatter2Name, get_user_name(get_single_chatter_uid(2)));
}

void chat_update(tick_t delta)
{
  if (get_uid() == 0xFF) {
    // Not initialized yet
    return;
  }

  // Poll on inbound events
  radio_poll();

  // Issue heartbeat
  heartbeat_countdown -= delta;
  if (heartbeat_countdown <= 0) {
    heartbeat_countdown = HEARTBEAT_INTERVAL;
    action_heartbeat();
    hb_counter[get_uid()] += 1;
    display_debug();
  }

  // Check heartbeat timeout
  for (radio_uid_t user = 0; user < USER_NUM; user++) {
    if (user == get_uid()) {
      continue;
    }
    if (user_online[user] && get_50hz_tick() - heartbeat_last_recv[user] >
                                 HEARTBEAT_OFFLINE_THRESHOLD) {
      user_online[user] = false;
      event_user_offline(user);
    }
  }
}
