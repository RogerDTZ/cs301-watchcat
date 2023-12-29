#include "app/chat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "sl_ui/ui.h"

#define USER_NAME_0 "Apple"
#define USER_NAME_1 "Banana"
#define USER_NAME_2 "Carrot"

LV_IMG_DECLARE(emoji_grin);
LV_IMG_DECLARE(emoji_rage);
LV_IMG_DECLARE(emoji_roll);

enum ui_session ui_curr_session;

static int heartbeat_countdown;
static tick_t heartbeat_last_recv[USER_NUM];
static bool user_online[USER_NUM];
static int hb_counter[USER_NUM];

struct radio_prot_msg chatter1_message_buffer[MAX_MESSAGE_BUFFER_LENGTH];
struct radio_prot_msg chatter2_message_buffer[MAX_MESSAGE_BUFFER_LENGTH];
struct radio_prot_msg group_message_buffer[MAX_MESSAGE_BUFFER_LENGTH];
lv_obj_t *chatter1_message_text[MAX_MESSAGE_BUFFER_LENGTH];
lv_obj_t *chatter2_message_text[MAX_MESSAGE_BUFFER_LENGTH];
lv_obj_t *group_message_text[MAX_MESSAGE_BUFFER_LENGTH];

int chatter1_message_buffer_pointer;
int chatter2_message_buffer_pointer;
int group_message_buffer_pointer;

int message_length;
uint8_t send_buffer[MSG_MAX_LEN];

static lv_obj_t *create_emoji(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                              const lv_img_dsc_t *img)
{
  lv_obj_t *obj;
  obj = lv_img_create(parent);
  lv_img_set_src(obj, img);
  lv_obj_set_pos(obj, x, y);
  lv_obj_set_align(obj, LV_ALIGN_CENTER);
  return obj;
}

static void free_emoji(lv_obj_t *emoji) { lv_obj_del(emoji); }

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

  // lv_label_set_text_fmt(ui_ChatDesc, "(%d, %d): %s", session, sender,
  // msg->msg);

  if (session == SESSION_ID_0_1_2) {
    group_message_buffer[group_message_buffer_pointer++] = *msg;
  } else if (sender == get_single_chatter_uid(1)) {
    chatter1_message_buffer[chatter1_message_buffer_pointer++] = *msg;
  } else {
    chatter2_message_buffer[chatter2_message_buffer_pointer++] = *msg;
  }

  update_current_message();
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

void update_current_message()
{
  if (ui_curr_session == UI_SESSION_CHAT1) {
    print_message(chatter1_message_text, chatter1_message_buffer,
                  chatter1_message_buffer_pointer);
  } else if (ui_curr_session == UI_SESSION_CHAT2) {
    print_message(chatter2_message_text, chatter2_message_buffer,
                  chatter2_message_buffer_pointer);
  } else if (ui_curr_session == UI_SESSION_GROUP) {
    print_message(group_message_text, group_message_buffer,
                  group_message_buffer_pointer);
  }
  if (ui_curr_session != UI_SESSION_CHAT1) {
    for (int i = 0; i < chatter1_message_buffer_pointer; i++) {
      if (chatter1_message_text[i]) {
        lv_obj_add_flag(chatter1_message_text[i], LV_OBJ_FLAG_HIDDEN);
      }
    }
  }
  if (ui_curr_session != UI_SESSION_CHAT2) {
    for (int i = 0; i < chatter2_message_buffer_pointer; i++) {
      if (chatter2_message_text[i]) {
        lv_obj_add_flag(chatter2_message_text[i], LV_OBJ_FLAG_HIDDEN);
      }
    }
  }
  if (ui_curr_session != UI_SESSION_GROUP) {
    for (int i = 0; i < group_message_buffer_pointer; i++) {
      if (group_message_text[i]) {
        lv_obj_add_flag(group_message_text[i], LV_OBJ_FLAG_HIDDEN);
      }
    }
  }
}

static int is_emo_prefix(const char *str)
{
  if (strlen(str) < 5) {
    return 0;
  }
  if (str[0] != 'e') {
    return 0;
  }
  if (str[1] != 'm') {
    return 0;
  }
  if (str[2] != 'o') {
    return 0;
  }
  if (str[3] != ':') {
    return 0;
  }
  if (str[4] == '1' || str[4] == '2' || str[4] == '3') {
    return str[4] - '0';
  }
  return 0;
}

static int create_message_box(lv_obj_t **text_obj,
                              const struct radio_prot_msg *prot_msg, int height)
{
  if (*text_obj == NULL) {
    int is_emo = is_emo_prefix(prot_msg->msg);

    int msg_len = strlen(prot_msg->msg);
    int lines = msg_len == 0 ? 1 : ((msg_len + 15) / 16);
    *text_obj = lv_label_create(ui_ChatsPanel);
    lv_obj_set_width(*text_obj, 100);
    lv_obj_set_height(*text_obj, lines * 26);
    lv_obj_set_align(*text_obj, LV_ALIGN_TOP_MID);
    lv_obj_set_y(*text_obj, height);
    if (is_emo == 0) {
      lv_label_set_text_fmt(*text_obj, "%s: %s", get_user_name(prot_msg->id),
                            prot_msg->msg);
    } else {
      lv_label_set_text_fmt(*text_obj, "%s: ", get_user_name(prot_msg->id));
    }
    lv_obj_set_style_text_font(*text_obj, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    if (is_emo) {
      create_emoji(*text_obj, 0, 5,
                   is_emo == 1   ? &emoji_grin
                   : is_emo == 2 ? &emoji_rage
                                 : &emoji_roll);
      lv_obj_set_height(*text_obj, 25);
    }
  }

  lv_obj_clear_flag(*text_obj, LV_OBJ_FLAG_HIDDEN);
  return lv_obj_get_height(*text_obj);
}

void print_message(lv_obj_t *texts[], struct radio_prot_msg buffer[],
                   int buffer_pointer)
{
  int height = 30;
  for (int i = 0; i < buffer_pointer; i++) {
    height += create_message_box(&texts[i], &buffer[i], height);
  }
}

bool is_user_online(radio_uid_t uid) { return user_online[uid]; }
