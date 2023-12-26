#ifndef __RADIO_H__
#define __RADIO_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t tick_t;
typedef uint8_t radio_uid_t;
typedef uint8_t radio_session_t;
typedef uint8_t radio_msg_len_t;
typedef void (*radio_outbound_callback_t)(void *body, bool success);

#define USER_NUM (3)

enum radio_prot_cmd {
  RADIO_PROT_CMD_HEARTBEAT,
  RADIO_PROT_CMD_MSG,
  RADIO_PROT_CMD_INVITE,
  RADIO_PROT_CMD_JOIN
};

struct radio_prot_msg {
  radio_uid_t id;          // messager's id
  radio_session_t session; // session of the message
  radio_msg_len_t len;
  const char *msg;
};

struct radio_prot_invite {
  radio_uid_t id;          // inviter's id
  radio_session_t session; // session to join
};

struct radio_prot_join {
  radio_uid_t id;          // joiner's id
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

enum radio_mode {
  RADIO_MODE_UNINIT,
  RADIO_MODE_RX,
  RADIO_MODE_TX,
};

/**
 * Initialize the NRF24L01 as a receiver.
 */
void radio_init_prx(radio_uid_t uid);

/**
 * Initialize the NRF24L01 as a transmitter.
 */
void radio_init_ptx(radio_uid_t uid_fr, radio_uid_t uid_to);

/**
 * IRQ dispatcher
 */
void radio_irq_dispatcher();

/**
 * Send a constructed radio protocol packet to a remote.
 * If callback is set, it will be call upon a packet is transmitted successfully
 * or failed to transferred, or timeout.
 * Timeout is ignored if set to 0.
 */
void radio_send(radio_uid_t id, const struct radio_prot_packet *pkt,
                tick_t timeout, radio_outbound_callback_t callback);

/**
 * Poll on packets received from the remote.
 * Return NULL if there is no inbound.
 * Corresponding handlers will be invoked.
 */
struct radio_prot_packet *radio_poll(radio_uid_t id);

// Extern handlers of radio inbound packet
// Structs will be freed after the call. So do not refer to them later

void radio_event_handler_heartbeat(radio_uid_t id);
void radio_event_handler_message(struct radio_prot_msg *prot_msg);
void radio_event_handler_invite(struct radio_prot_invite *prot_invite);
void radio_event_handler_join(struct radio_prot_invite *prot_join);

void radio_test_set_mode(int id);
void radio_test_send(int target);

#endif /* __RADIO_H__ */
