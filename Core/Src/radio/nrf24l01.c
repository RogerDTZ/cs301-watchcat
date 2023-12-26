#include <assert.h>

#include "spi.h"

#include "BSP/NRF24L01/24l01.h"
#include "sl_ui/ui.h"

#include "radio/radio.h"

#define RADIO_CONF_CH (55)

#define PIPE_NUM     (6)
#define PIPE_ENC_ALL ((1 << PIPE_NUM) - 1)

const uint8_t RADIO_CONF_PIPE_ADDR_0[] = {0x33, 0xe3, 0x13, 0x13, 0x3a};
const uint8_t RADIO_CONF_PIPE_ADDR_1[] = {0x34, 0xe3, 0x13, 0x13, 0x3a};
const uint8_t RADIO_CONF_PIPE_ADDR_2[] = {0x35, 0xe3, 0x13, 0x13, 0x3a};
const uint8_t RADIO_CONF_PIPE_ADDR_3[] = {0x36, 0xe3, 0x13, 0x13, 0x3a};
const uint8_t RADIO_CONF_PIPE_ADDR_4[] = {0x37, 0xe3, 0x13, 0x13, 0x3a};
const uint8_t RADIO_CONF_PIPE_ADDR_5[] = {0x38, 0xe3, 0x13, 0x13, 0x3a};

static enum radio_mode radio_mode = RADIO_MODE_UNINIT;

inline static uint8_t pipe_encoded(uint8_t id) { return 1 << id; }

static uint8_t pipe_connecting(radio_user_t x, radio_user_t y)
{
  assert(0 <= x && x < CLIENT_NUM);
  assert(0 <= y && y < CLIENT_NUM);

  if ((x == 0 && y == 1) || (x == 1 && y == 0)) {
    return 0 + !(x > y);
  } else if ((x == 0 && y == 2) || (x == 2 && y == 0)) {
    return 2 + (x > y);
  } else {
    return 4 + (x > y);
  }
}

static uint8_t pipe_sender(uint8_t id)
{
  switch (id) {
  case 0:
    return 1;
  case 1:
    return 0;
  case 2:
    return 0;
  case 3:
    return 2;
  case 4:
    return 1;
  case 5:
    return 2;
  default:
    break;
  }
  return 0xFF;
}

static uint8_t pipes_related(radio_user_t x)
{
  uint8_t code = 0;
  for (radio_user_t y = 0; y < CLIENT_NUM; y++) {
    if (y != x) {
      code |= pipe_encoded(pipe_connecting(y, x));
    }
  }
  return code;
}

static uint8_t *get_pipe_addr(uint8_t id)
{
  assert(0 <= id && id < PIPE_NUM);

  switch (id) {
  case 0:
    return RADIO_CONF_PIPE_ADDR_0;
  case 1:
    return RADIO_CONF_PIPE_ADDR_1;
  case 2:
    return RADIO_CONF_PIPE_ADDR_2;
  case 3:
    return RADIO_CONF_PIPE_ADDR_3;
  case 4:
    return RADIO_CONF_PIPE_ADDR_4;
  case 5:
    return RADIO_CONF_PIPE_ADDR_5;
  }
  return NULL;
}

void radio_init_prx(radio_user_t user)
{
  assert(0 <= user && user < CLIENT_NUM);

  uint8_t recv_pipe_bits = pipes_related(user);

  NRF24L01_CE = 0;

  // EN_CRC, CRCO: 2 bytes, PWR_UP, PRX
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0b00001111);
  // Enable auto-ack
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, recv_pipe_bits);
  // Enable & configure RX address
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, recv_pipe_bits);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0,
                     (uint8_t *)RADIO_CONF_PIPE_ADDR_0, RX_ADR_WIDTH);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P1,
                     (uint8_t *)RADIO_CONF_PIPE_ADDR_1, RX_ADR_WIDTH);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P2,
                     (uint8_t *)RADIO_CONF_PIPE_ADDR_2, 1);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P2,
                     (uint8_t *)RADIO_CONF_PIPE_ADDR_3, 1);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P4,
                     (uint8_t *)RADIO_CONF_PIPE_ADDR_4, 1);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P5,
                     (uint8_t *)RADIO_CONF_PIPE_ADDR_5, 1);
  // Set RF channel
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, RADIO_CONF_CH);
  // Data rates = 2 Mbps, output power = 0 dBm
  NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0b00001110);
  // Clear status
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, 0b01110000);
  // Configure default RX payload length to 32 bytes
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P1, RX_PLOAD_WIDTH);
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P2, RX_PLOAD_WIDTH);
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P3, RX_PLOAD_WIDTH);
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P4, RX_PLOAD_WIDTH);
  NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P5, RX_PLOAD_WIDTH);
  // Enable features: none
  NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, 0b00000000);

  NRF24L01_CE = 1; // CE为高,进入接收模式

  radio_mode = RADIO_MODE_RX;
}

void radio_init_ptx(radio_user_t user_from, radio_user_t user_to)
{
  assert(0 <= user_from && user_from < CLIENT_NUM);
  assert(0 <= user_to && user_to < CLIENT_NUM);

  const uint8_t *pipe_addr = get_pipe_addr(pipe_connecting(user_from, user_to));

  NRF24L01_CE = 0;

  // EN_CRC, CRCO: 2 bytes, PWR_UP, PTX
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0b00001110);
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
  // Enable features: none
  NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, 0b00000000);

  NRF24L01_CE = 1; // CE为高,进入接收模式

  radio_mode = RADIO_MODE_TX;
}

void radio_accept_inbound_packet()
{
  static uint8_t rx_buf[RX_PLOAD_WIDTH + 5] = "X: ";
  uint8_t rx_pipe;

  if (radio_mode != RADIO_MODE_RX) {
    // mode not matched
    return;
  }

  if (NRF24L01_RxPacket(rx_buf + 3, &rx_pipe) == 0) {
    rx_buf[0] = '0' + pipe_sender(rx_pipe);
    rx_buf[3 + 32] = 0;
    lv_label_set_text(ui_CalcFml, rx_buf);
  }
}
