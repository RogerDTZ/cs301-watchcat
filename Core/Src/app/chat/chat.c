#include "app/chat.h"

#include "radio/radio.h"

void radio_event_handler_heartbeat(struct radio_prot_heartbeat *prot_heartbeat)
{
}

void radio_event_handler_message(struct radio_prot_msg *prot_msg) {}

void radio_event_handler_invite(struct radio_prot_invite *prot_invite) {}

void radio_event_handler_join(struct radio_prot_join *prot_join) {}
