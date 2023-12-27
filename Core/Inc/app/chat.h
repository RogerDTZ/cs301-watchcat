#ifndef __CHAT_H__
#define __CHAT_H__

#include "radio/radio.h"

#define SESSION_ID_0_1   (0)
#define SESSION_ID_0_2   (1)
#define SESSION_ID_1_2   (2)
#define SESSION_ID_0_1_2 (3)

// Broadcast heartbeat every certain period (ms)
#define HEARTBEAT_INTERVAL (500)
// If not received heartbeat for a certain period (ms), consider the remote
// offline
#define HEARTBEAT_OFFLINE_THRESHOLD (1500)

radio_uid_t get_session_with(radio_uid_t uid);

radio_uid_t get_chatter_from_session(radio_session_t session);

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
void event_user_online(radio_uid_t uid);

/**
 * Called when a user turns offline.
 */
void event_user_offline(radio_uid_t uid);

/**
 * Init chat app with the specified identity.
 */
void chat_init(radio_uid_t uid);

/**
 * Check the activity of the last period. Issue heartbeat if necessary.
 * Poll on radio inbound.
 */
void chat_update(tick_t delta);

#endif /* __CHAT_H__ */
