#ifndef __CHAT_H__
#define __CHAT_H__

#include "radio/radio.h"

#define SESSION_ID_0_1   (0)
#define SESSION_ID_0_2   (1)
#define SESSION_ID_1_2   (2)
#define SESSION_ID_0_1_2 (3)

/**
 * Invite somebody into a chat
 */
void action_invite(radio_uid_t uid, radio_session_t session);

#endif /* __CHAT_H__ */
