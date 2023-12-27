#ifndef __CHAT_H__
#define __CHAT_H__

#include "radio/radio.h"

#define SESSION_ID_0_1   (0)
#define SESSION_ID_0_2   (1)
#define SESSION_ID_1_2   (2)
#define SESSION_ID_0_1_2 (3)

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

#endif /* __CHAT_H__ */
