#include <assert.h>

#include "spi.h"

#include "BSP/NRF24L01/24l01.h"

#include "radio/radio.h"

#define RADIO_CONF_CH (55)

#define PIPE_NUM     (3)
#define PIPE_ENC_ALL ((1 << PIPE_NUM) - 1)

typedef uint8_t pipe_t;

const uint8_t RADIO_CONF_PIPE_ADDR_0[] = {0x33, 0xe3, 0x13, 0x13, 0x3a};
const uint8_t RADIO_CONF_PIPE_ADDR_1[] = {0x34, 0xe3, 0x13, 0x13, 0x3a};
const uint8_t RADIO_CONF_PIPE_ADDR_2[] = {0x35, 0xe3, 0x13, 0x13, 0x3a};

static enum radio_mode radio_mode = RADIO_MODE_UNINIT;
static radio_uid_t local_uid;
radio_uid_t tx_uid_fr;
radio_uid_t tx_uid_to;

inline static uint8_t pipe_encode(pipe_t pid) { return 1u << pid; }

static uint8_t pipe_connecting(radio_uid_t x, radio_uid_t y)
{
  assert(0 <= x && x < USER_NUM);
  assert(0 <= y && y < USER_NUM);

  if ((x == 0 && y == 1) || (x == 1 && y == 0)) {
    return 0;
  } else if ((x == 0 && y == 2) || (x == 2 && y == 0)) {
    return 1;
  } else {
    return 2;
  }
}

static radio_uid_t get_pipe_sender(uint8_t id)
{
  switch (id) {
  case 0:
    assert(local_uid == 0 || local_uid == 1);
    return local_uid == 0 ? 1 : 0;
  case 1:
    assert(local_uid == 0 || local_uid == 2);
    return local_uid == 0 ? 2 : 0;
  case 2:
    assert(local_uid == 1 || local_uid == 2);
    return local_uid == 1 ? 2 : 1;
  default:
    break;
  }
  return 0xFF;
}

static uint8_t pipes_related(radio_uid_t x)
{
  uint8_t code = 0;
  for (radio_uid_t y = 0; y < USER_NUM; y++) {
    if (y != x) {
      code |= pipe_encode(pipe_connecting(y, x));
    }
  }
  return code;
}

static const uint8_t *get_pipe_addr(uint8_t id)
{
  assert(0 <= id && id < PIPE_NUM);

  switch (id) {
  case 0:
    return RADIO_CONF_PIPE_ADDR_0;
  case 1:
    return RADIO_CONF_PIPE_ADDR_1;
  case 2:
    return RADIO_CONF_PIPE_ADDR_2;
    // case 3:
    //   return RADIO_CONF_PIPE_ADDR_3;
    // case 4:
    //   return RADIO_CONF_PIPE_ADDR_4;
    // case 5:
    //   return RADIO_CONF_PIPE_ADDR_5;
  }
  return NULL;
}

static void init_rx_pipes(uint8_t enabled_pipes)
{
  for (pipe_t pid = 0; pid < PIPE_NUM; pid++) {
    if ((enabled_pipes >> pid) & 1) {
      // Set pipe address
      if (pid < 2) {
        NRF24L01_Write_Buf(NRF_WRITE_REG + (RX_ADDR_P0 + pid),
                           get_pipe_addr(pid), RX_ADR_WIDTH);
      } else {
        NRF24L01_Write_Reg(NRF_WRITE_REG + (RX_ADDR_P0 + pid),
                           get_pipe_addr(pid)[0]);
      }
      // Set payload width
      NRF24L01_Write_Reg(NRF_WRITE_REG + (RX_PW_P0 + pid), RX_PLOAD_WIDTH);
    }
  }
}

void radio_init_prx(radio_uid_t uid)
{
  assert(0 <= uid && uid < USER_NUM);

  uint8_t recv_pipe_bits = pipes_related(uid);

  NRF24L01_CE = 0;

  // EN_CRC, CRCO: 2 bytes, PWR_UP, PRX
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0b00001111);
  // Enable auto-ack
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, recv_pipe_bits);
  // Enable RX address
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, recv_pipe_bits);
  // Set address width = 5 bytes
  NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_AW, 0b00000011);
  // Set RF channel
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, RADIO_CONF_CH);
  // Data rates = 2 Mbps, output power = 0 dBm
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0b00001110);
  // Clear status
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, 0b01110000);
  // Set RX address & payload width
  init_rx_pipes(recv_pipe_bits);
  // Enable features: none
  NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, 0b00000000);

  NRF24L01_CE = 1; // CE为高,进入接收模式

  local_uid = uid;
  radio_mode = RADIO_MODE_RX;
}

void radio_init_ptx(radio_uid_t uid_fr, radio_uid_t uid_to)
{
  assert(0 <= uid_fr && uid_fr < USER_NUM);
  assert(0 <= uid_to && uid_to < USER_NUM);

  tx_uid_fr = uid_fr;
  tx_uid_to = uid_to;

  const uint8_t *pipe_addr = get_pipe_addr(pipe_connecting(uid_fr, uid_to));

  NRF24L01_CE = 0;

  // EN_CRC, CRCO: 2 bytes, PWR_UP, PTX
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0b00001110);
  // Disable auto-ack
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0b00000001);
  // Enable only pipe 0
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0b00000001);
  // Set address width = 5 bytes
  NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_AW, 0b00000011);
  // Set ARD = 500 uS, ARC = 10
  NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0b00011010);
  // Set RF channel
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, RADIO_CONF_CH);
  // Data rates = 2 Mbps, output power = 0 dBm
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0b00001110);
  // Clear status
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, 0b01110000);
  // Configure TX addr and RX addr (for ACK)
  NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, pipe_addr, TX_ADR_WIDTH);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, pipe_addr, RX_ADR_WIDTH);
  // Set payload width
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);
  // Enable features: none
  NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, 0b00000000);

  NRF24L01_CE = 1; // CE为高,进入接收模式

  local_uid = uid_fr;
  radio_mode = RADIO_MODE_TX;
}

void radio_enable_rx_irq(bool enable)
{
  NRF24L01_CE = 0;

  uint8_t config;

  if (!enable) {
    config = NRF24L01_Read_Reg(CONFIG);
    config &= ~MASK_RX_DR;
    NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, config);
  } else {
    config = NRF24L01_Read_Reg(CONFIG);
    config |= MASK_RX_DR;
    NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, config);
  }

  NRF24L01_CE = 1;

  // Handle pending interrupts
  if (enable) {
    radio_irq_dispatcher();
  }
}

void radio_irq_dispatcher()
{
  uint8_t status = NRF24L01_Read_Reg(STATUS);

  // Detect inbound payload
  if (radio_mode == RADIO_MODE_RX && (status & RX_OK)) {
    uint8_t rx_pipe = (status & STATUS_RX_P_NO_MASK) >> 1;
    radio_uid_t sender = get_pipe_sender(rx_pipe);

    static uint8_t rx_payload[RX_PLOAD_WIDTH];
    NRF24L01_Read_Buf(RD_RX_PLOAD, rx_payload, RX_PLOAD_WIDTH);
    NRF24L01_Write_Reg(FLUSH_RX, 0xFF);

    radio_handle_inbound(sender, rx_payload);

    // Clear interrupts
    NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, RX_OK);
  }
}
