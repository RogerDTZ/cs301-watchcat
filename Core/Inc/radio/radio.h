#ifndef __RADIO_H__
#define __RADIO_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t tick_t;
typedef uint16_t pkt_size_t;
typedef uint8_t radio_uid_t;
typedef uint8_t radio_session_t;
typedef uint8_t radio_msg_len_t;
typedef void (*radio_outbound_callback_t)(void *body, bool success);

#define USER_NUM (3)

#define MSG_MAX_LEN (30)

enum radio_prot_cmd {
  RADIO_PROT_CMD_HEARTBEAT,
  RADIO_PROT_CMD_MSG,
  RADIO_PROT_CMD_INVITE,
  RADIO_PROT_CMD_JOIN,
};

struct radio_prot_heartbeat {
  radio_uid_t id;
};

struct radio_prot_msg {
  radio_uid_t id;          // messager's id
  radio_session_t session; // session of the message
  radio_msg_len_t len;
  char msg[MSG_MAX_LEN];
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
  union {
    struct radio_prot_heartbeat heartbeat;
    struct radio_prot_msg msg;
    struct radio_prot_invite invite;
    struct radio_prot_join join;
  } body;
};

enum radio_mode {
  RADIO_MODE_UNINIT,
  RADIO_MODE_RX,
  RADIO_MODE_TX,
};

/**
 * Ultimate init.
 */
void radio_init(radio_uid_t uid);

radio_uid_t get_uid();

/**
 * Initialize the NRF24L01 as a receiver.
 */
void radio_init_prx(radio_uid_t uid);

/**
 * Initialize the NRF24L01 as a transmitter.
 */
void radio_init_ptx(radio_uid_t uid_fr, radio_uid_t uid_to);

void radio_enable_rx_irq(bool enable);

/**
 * IRQ dispatcher
 */
void radio_irq_dispatcher();

/**
 * Accept inbound radio payload
 */
void radio_handle_inbound(radio_uid_t sender, const uint8_t *rx_payload);

/**
 * Send a constructed radio protocol packet to a remote.
 * If callback is set, it will be call upon a packet is transmitted successfully
 * or failed to transferred, or timeout.
 * @return true for success, 1 for try later
 */
bool radio_send(radio_uid_t uid, const struct radio_prot_packet *pkt);

/**
 * Send a constructed radio protocol packet to all remotes.
 */
bool radio_broadcast(const struct radio_prot_packet *pkt);

/**
 * Poll on packets received from the remote.
 * Return NULL if there is no inbound.
 * Corresponding handlers will be invoked.
 */
void radio_poll();

// Extern handlers of radio inbound packet
// Structs will be freed after the call. So do not refer to them later

void radio_event_handler_heartbeat(struct radio_prot_heartbeat *prot_heartbeat);
void radio_event_handler_message(struct radio_prot_msg *prot_msg);
void radio_event_handler_invite(struct radio_prot_invite *prot_invite);
void radio_event_handler_join(struct radio_prot_join *prot_join);

#endif /* __RADIO_H__ */
