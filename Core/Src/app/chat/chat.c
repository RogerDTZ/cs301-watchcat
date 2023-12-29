#include "app/chat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "sl_ui/ui.h"

#define USER_NAME_0 "Apple"
#define USER_NAME_1 "Banana"
#define USER_NAME_2 "Carrot"

struct emoji {
  lv_obj_t *img;
};

LV_IMG_DECLARE(emoji_grin);
LV_IMG_DECLARE(emoji_rage);
LV_IMG_DECLARE(emoji_roll);

enum ui_session ui_curr_session;

static int heartbeat_countdown;
static tick_t heartbeat_last_recv[USER_NUM];
static bool user_online[USER_NUM];
static int hb_counter[USER_NUM];

static struct emoji *create_emoji(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                                  const lv_img_dsc_t *img)
{
  struct emoji *emoji = malloc(sizeof(struct emoji));
  emoji->img = lv_img_create(parent);
  lv_img_set_src(emoji->img, img);
  lv_obj_set_pos(emoji->img, x, y);
  return emoji;
}

static void free_emoji(struct emoji *emoji)
{
  lv_obj_del(emoji->img);
  free(emoji);
}

static void display_chat_bg()
{
  lv_label_set_text_fmt(ui_ChatBgInfo, "[%s] %d %d %d",
                        get_user_name(get_uid()), hb_counter[0], hb_counter[1],
                        hb_counter[2]);
}

/**
 * Init chat app with the specified identity.
 */
static void chat_init(radio_uid_t uid)
{
  radio_init(uid);

  heartbeat_countdown = 0;
  memset(heartbeat_last_recv, 0, sizeof(heartbeat_last_recv));
  memset(user_online, 0, sizeof(user_online));

  ui_curr_session = UI_SESSION_NONE;

  lv_label_set_text(ui_Chatter1Name, get_user_name(get_single_chatter_uid(1)));
  lv_label_set_text(ui_Chatter2Name, get_user_name(get_single_chatter_uid(2)));
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

  hb_counter[sender] = (hb_counter[sender] + 1) % 10;
  display_chat_bg();

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
    hb_counter[get_uid()] = (hb_counter[get_uid()] + 1) % 10;
    display_chat_bg();
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

static void show_id_selection_interface()
{
  lv_obj_clear_flag(ui_ChatIDSelection, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_ChattersPanel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_ChatsPanel, LV_OBJ_FLAG_HIDDEN);
}

static void show_chat_interface()
{
  lv_obj_add_flag(ui_ChatIDSelection, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ui_ChattersPanel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ui_ChatsPanel, LV_OBJ_FLAG_HIDDEN);
}

void event_selected_id(int id)
{
  assert(0 <= id && id < USER_NUM && "Invalid id");

  chat_init(id);

  show_chat_interface();
}

void open_app_chat()
{
  lv_obj_clear_flag(ui_ChatApp, LV_OBJ_FLAG_HIDDEN);

  if (get_uid() == 0xFF) {
    // Not initialized yet
    show_id_selection_interface();
  } else {
    show_chat_interface();
  }
}

void close_app_chat() { lv_obj_add_flag(ui_ChatApp, LV_OBJ_FLAG_HIDDEN); }
