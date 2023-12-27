#include "app/chat.h"

void action_invite(radio_uid_t uid, radio_session_t session)
{
  struct radio_prot_packet pkt;
  pkt.cmd = RADIO_PROT_CMD_INVITE;
  pkt.body.invite.id = get_uid();
  pkt.body.invite.session = session;

  radio_send(uid, &pkt);
}

void radio_event_handler_heartbeat(struct radio_prot_heartbeat *prot_heartbeat)
{
}

void radio_event_handler_message(struct radio_prot_msg *prot_msg) {}

void radio_event_handler_invite(struct radio_prot_invite *prot_invite) {}

void radio_event_handler_join(struct radio_prot_join *prot_join) {}
