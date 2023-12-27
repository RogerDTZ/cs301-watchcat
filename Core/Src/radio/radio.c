#include "radio/radio.h"

#include <stdlib.h>
#include <string.h>

#include "SYSTEM/delay/delay.h"
#include "main.h"

#include "BSP/NRF24L01/24l01.h"

#include "util/circular_queue.h"

#define TX_RETRY_CNT (10)

#define OUTBOUND_BUFFER_SIZE (64)
#define CIRCULAR_BUFFER_SIZE (256)

static const uint16_t magic_number = 0x1353u;

#define CHUNK_PAYLOAD_SIZE (TX_PLOAD_WIDTH - sizeof(magic_number))

static radio_uid_t local_uid = 0xFF; // Uninited
extern radio_uid_t tx_uid_fr;
extern radio_uid_t tx_uid_to;

static uint8_t tx_payload[TX_PLOAD_WIDTH];

static circular_queue_t *q_in;

static pkt_size_t get_packet_size(const struct radio_prot_packet *pkt)
{
  pkt_size_t size = sizeof(pkt->cmd);
  switch (pkt->cmd) {
  case RADIO_PROT_CMD_HEARTBEAT:
    return size + sizeof(struct radio_prot_heartbeat);
  case RADIO_PROT_CMD_MSG:
    return size + sizeof(struct radio_prot_msg);
  case RADIO_PROT_CMD_INVITE:
    return size + sizeof(struct radio_prot_invite);
  case RADIO_PROT_CMD_JOIN:
    return size + sizeof(struct radio_prot_join);
  default:
    break;
  }
  assert(0 && "Unknown command");
}

/**
 * Return bytes transmitted
 */
static pkt_size_t radio_send_transmit(uint8_t out_buf[], pkt_size_t size)
{
  // Send payloads
  size_t chunk_size;
  uint8_t *curr = out_buf;
  *(uint16_t *)tx_payload = magic_number;
  while (size > 0) {
    chunk_size = (size <= CHUNK_PAYLOAD_SIZE) ? size : CHUNK_PAYLOAD_SIZE;
    memcpy(tx_payload + sizeof(magic_number), curr, chunk_size);
    memset(tx_payload + sizeof(magic_number) + chunk_size, 0,
           CHUNK_PAYLOAD_SIZE - chunk_size);

    bool success = false;
    for (int t = 0; t < TX_RETRY_CNT; t++) {
      int res = NRF24L01_TxPacket(tx_payload);
      if (res & TX_OK) {
        success = true;
        break;
      }
      // Switch back to PRX mode for a while
      radio_init_prx(local_uid);
      delay_ms(rand() % 10 + 1);
      radio_init_ptx(tx_uid_fr, tx_uid_to);
    }
    if (!success) {
      break;
    }

    curr += chunk_size;
    size -= chunk_size;
  }
  return curr - out_buf;
}

/**
 * Return true if success
 */
static bool radio_send_worker_single(radio_uid_t uid, uint8_t out_buf[],
                                     pkt_size_t size)
{
  // Switch to PTX mode
  radio_init_ptx(local_uid, uid);

  pkt_size_t res = radio_send_transmit(out_buf, size);

  // Switch back to PRX mode
  radio_init_prx(local_uid);

  return res == size;
}

/**
 * Return true if success
 */
static bool radio_send_worker_broadcast(uint8_t out_buf[], pkt_size_t size)
{
  bool failed = false;
  for (radio_uid_t uid = 0; uid < USER_NUM; uid++) {
    if (uid == local_uid) {
      continue;
    }
    // Switch to PTX mode
    radio_init_ptx(local_uid, uid);

    pkt_size_t res = radio_send_transmit(out_buf, size);
    failed |= (res < size);
  }

  // Switch back to PRX mode
  radio_init_prx(local_uid);

  return !failed;
}

/**
 * For this internal worker, uid == 0xFF stands for broadcast.
 */
static bool radio_send_worker(radio_uid_t uid,
                              const struct radio_prot_packet *pkt)
{
  pkt_size_t size = get_packet_size(pkt);
  assert(size < OUTBOUND_BUFFER_SIZE && "Packet too large");

  static uint8_t out_buf[OUTBOUND_BUFFER_SIZE];
  uint8_t *curr = out_buf;
  // command
  *(enum radio_prot_cmd *)curr = pkt->cmd;
  curr += sizeof(pkt->cmd);
  // body
  switch (pkt->cmd) {
  case RADIO_PROT_CMD_HEARTBEAT:
    *(struct radio_prot_heartbeat *)curr = pkt->body.heartbeat;
    curr += sizeof(pkt->body.heartbeat);
    break;
  case RADIO_PROT_CMD_MSG:
    *(struct radio_prot_msg *)curr = pkt->body.msg;
    curr += sizeof(pkt->body.msg);
    break;
  case RADIO_PROT_CMD_INVITE:
    *(struct radio_prot_invite *)curr = pkt->body.invite;
    curr += sizeof(pkt->body.invite);
    break;
  case RADIO_PROT_CMD_JOIN:
    *(struct radio_prot_join *)curr = pkt->body.join;
    curr += sizeof(pkt->body.join);
    break;
  default:
    assert(0 && "Unknown command");
  }
  assert(curr - out_buf == size && "Packet size mismatch");

  if (uid < 0xFF) {
    assert(uid < USER_NUM);
    return radio_send_worker_single(uid, out_buf, size);
  } else {
    // broadcast
    return radio_send_worker_broadcast(out_buf, size);
  }
}

void radio_init(radio_uid_t uid)
{
  local_uid = uid;

  q_in = circular_queue_alloc(CIRCULAR_BUFFER_SIZE);

  radio_init_prx(uid);
}

radio_uid_t get_uid() { return local_uid; }

void radio_handle_inbound(radio_uid_t sender, const uint8_t *rx_payload)
{
  // Don't care the sender
  UNUSED(sender);

  if (*(uint16_t *)rx_payload != magic_number) {
    // Not a valid packet
    return;
  }

  pkt_size_t size =
      circular_queue_push(q_in, RX_PLOAD_WIDTH - sizeof(magic_number),
                          rx_payload + sizeof(magic_number));
  assert(size == RX_PLOAD_WIDTH - sizeof(magic_number) &&
         "Inbound buffer full");
}

bool radio_send(radio_uid_t uid, const struct radio_prot_packet *pkt)
{
  assert(local_uid != 0xFF && "Radio not initialized");
  assert(uid < USER_NUM && uid != local_uid && "Invalid uid for send");

  // Re-init random seed for half-duplex randomness
  srand(get_50hz_tick());

  return radio_send_worker(uid, pkt);
}

bool radio_broadcast(const struct radio_prot_packet *pkt)
{
  return radio_send_worker(0xFF, pkt);
}

void radio_poll()
{
  struct radio_prot_packet pkt;

  // Disable RX inbound IRQ, which may modify the queue
  radio_enable_rx_irq(false);

  while (!circular_queue_empty(q_in)) {
    int q_size = circular_queue_size(q_in);
    if (q_size < sizeof(pkt.cmd)) {
      break;
    }
    circular_queue_get(q_in, sizeof(pkt.cmd), &pkt.cmd);
    switch (pkt.cmd) {
    case RADIO_PROT_CMD_HEARTBEAT:
      if (q_size >= sizeof(pkt.cmd) + sizeof(pkt.body.heartbeat)) {
        circular_queue_pop(q_in, sizeof(pkt.cmd));
        circular_queue_get(q_in, sizeof(pkt.body.heartbeat),
                           &pkt.body.heartbeat);
        circular_queue_pop(q_in, sizeof(pkt.body.heartbeat));
        radio_event_handler_heartbeat(&pkt.body.heartbeat);
        continue;
      }
      break;
    case RADIO_PROT_CMD_MSG:
      if (q_size >= sizeof(pkt.cmd) + sizeof(pkt.body.msg)) {
        circular_queue_pop(q_in, sizeof(pkt.cmd));
        circular_queue_get(q_in, sizeof(pkt.body.msg), &pkt.body.msg);
        circular_queue_pop(q_in, sizeof(pkt.body.msg));
        radio_event_handler_message(&pkt.body.msg);
        continue;
      }
      break;
    case RADIO_PROT_CMD_INVITE:
      if (q_size >= sizeof(pkt.cmd) + sizeof(pkt.body.invite)) {
        circular_queue_pop(q_in, sizeof(pkt.cmd));
        circular_queue_get(q_in, sizeof(pkt.body.invite), &pkt.body.invite);
        circular_queue_pop(q_in, sizeof(pkt.body.invite));
        radio_event_handler_invite(&pkt.body.invite);
        continue;
      }
      break;
    case RADIO_PROT_CMD_JOIN:
      if (q_size >= sizeof(pkt.cmd) + sizeof(pkt.body.join)) {
        circular_queue_pop(q_in, sizeof(pkt.cmd));
        circular_queue_get(q_in, sizeof(pkt.body.join), &pkt.body.join);
        circular_queue_pop(q_in, sizeof(pkt.body.join));
        radio_event_handler_join(&pkt.body.join);
        continue;
      }
      break;
    }
    // No packet decoded, end the polling
    break;
  }

  radio_enable_rx_irq(true);
}
