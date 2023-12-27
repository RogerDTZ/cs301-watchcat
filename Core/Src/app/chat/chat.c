#include "app/chat.h"

#include <string.h>

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

void radio_event_handler_heartbeat(struct radio_prot_heartbeat *heartbeat) {}

void radio_event_handler_message(struct radio_prot_msg *msg) {}

void radio_event_handler_invite(struct radio_prot_invite *invite) {}
