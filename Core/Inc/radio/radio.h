#ifndef __RADIO_H__
#define __RADIO_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t tick_t;
typedef uint8_t radio_user_t;
typedef uint8_t radio_session_t;
typedef uint8_t radio_msg_len_t;
typedef void (*radio_outbound_callback_t)(void *body, bool success);

#define CLIENT_NUM (3)

enum radio_prot_cmd {
  RADIO_PROT_CMD_HEARTBEAT,
  RADIO_PROT_CMD_MSG,
  RADIO_PROT_CMD_INVITE,
  RADIO_PROT_CMD_JOIN
};

struct radio_prot_msg {
  radio_user_t id;         // messager's id
  radio_session_t session; // session of the message
  radio_msg_len_t len;
  const char *msg;
};

struct radio_prot_invite {
  radio_user_t id;         // inviter's id
  radio_session_t session; // session to join
};

struct radio_prot_join {
  radio_user_t id;         // joiner's id
  radio_session_t session; // session to join
};

struct radio_prot_packet {
  enum radio_prot_cmd cmd;
  uint16_t body_len;
  void *body;
};

struct radio_prot_outbound {
  struct radio_prot_packet pkt;
  tick_t tick_issued;
  tick_t timeout;
  radio_outbound_callback_t callback;
};

void radio_init_prx(radio_user_t user);
void radio_init_ptx(radio_user_t user_from, radio_user_t user_to);

/**
 * Initialize the NRF24L01 hardware.
 * Initiate the radio with the specified user ID.
 */
void radio_init(radio_user_t id);

/**
 * Send a constructed radio protocol packet to a remote.
 * If callback is set, it will be call upon a packet is transmitted successfully
 * or failed to transferred, or timeout.
 * Timeout is ignored if set to 0.
 */
void radio_send(radio_user_t id, const struct radio_prot_packet *pkt,
                tick_t timeout, radio_outbound_callback_t callback);

/**
 * Poll on packets received from the remote.
 * Return NULL if there is no inbound.
 * Corresponding handlers will be invoked.
 */
struct radio_prot_packet *radio_poll(radio_user_t id);

// Extern handlers of radio inbound packet
// Structs will be freed after the call. So do not refer to them later

void radio_event_handler_heartbeat(radio_user_t id);
void radio_event_handler_message(struct radio_prot_msg *prot_msg);
void radio_event_handler_invite(struct radio_prot_invite *prot_invite);
void radio_event_handler_join(struct radio_prot_invite *prot_join);

#endif /* __RADIO_H__ */
