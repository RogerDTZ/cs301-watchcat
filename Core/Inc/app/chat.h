#ifndef __CHAT_H__
#define __CHAT_H__

#include "lvgl.h"

#include "radio/radio.h"

#define SESSION_ID_0_1   (0)
#define SESSION_ID_0_2   (1)
#define SESSION_ID_1_2   (2)
#define SESSION_ID_0_1_2 (3)

// Broadcast heartbeat every certain period (ms)
#define HEARTBEAT_INTERVAL (1000)
// If not received heartbeat for a certain period (ms), consider the remote
// offline
#define HEARTBEAT_OFFLINE_THRESHOLD (2500)

#define MAX_MESSAGE_BUFFER_LENGTH (20)

enum ui_session {
  UI_SESSION_NONE,
  UI_SESSION_CHAT1,
  UI_SESSION_CHAT2,
  UI_SESSION_GROUP,
};

extern enum ui_session ui_curr_session;

extern int message_length;
extern uint8_t send_buffer[MSG_MAX_LEN];

extern struct radio_prot_msg chatter1_message_buffer[MAX_MESSAGE_BUFFER_LENGTH];

extern struct radio_prot_msg chatter2_message_buffer[MAX_MESSAGE_BUFFER_LENGTH];

extern struct radio_prot_msg group_message_buffer[MAX_MESSAGE_BUFFER_LENGTH];

extern int chatter1_message_buffer_pointer;

extern int chatter2_message_buffer_pointer;

extern int group_message_buffer_pointer;

const char *get_user_name(radio_uid_t uid);

radio_uid_t get_session_with(radio_uid_t uid);

radio_uid_t get_chatter_from_session(radio_session_t session);

/**
 * @brief Return the uid of the chatter in the UI
 *
 * @param id 1 or 2, indicating the first or second single-chat
 */
radio_uid_t get_single_chatter_uid(int ui_id);

/**
 * Broadcast heartbeat to everyone.
 * Only call it when there's nothing to send in the last period.
 */
void action_heartbeat();

/**
 * Send a message to a specific session.
 */
void action_message(radio_session_t session, const char *msg);

/**
 * Invite somebody into a chat.
 * Return true if invitation is successfully sent.
 */
bool action_invite(radio_uid_t uid, radio_session_t session);

/**
 * Called when a user turns online.
 */
void event_user_online(radio_uid_t uid, bool notify);

/**
 * Called when a user turns offline.
 */
void event_user_offline(radio_uid_t uid);

/**
 * Check the activity of the last period. Issue heartbeat if necessary.
 * Poll on radio inbound.
 */
void chat_update(tick_t delta);

void open_app_chat();

void close_app_chat();

void event_selected_id(int id);

void update_current_message();

bool is_user_online(radio_uid_t uid);

void print_message(lv_obj_t *texts[], struct radio_prot_msg buffer[],
                   int buffer_pointer);

#endif /* __CHAT_H__ */
