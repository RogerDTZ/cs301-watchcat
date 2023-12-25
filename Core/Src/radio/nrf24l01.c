#include <assert.h>

#include "BSP/NRF24L01/24l01.h"

#include "radio/radio.h"

#define RADIO_CONF_CH (55)

#define PIPE_CNT      (3)
#define PIPE_CODE_ALL ((1 << PIPE_CNT) - 1)

const uint8_t RADIO_CONF_PIPE_ADDR_0[] = {0x13, 0xe3, 0x13, 0x13, 0x33};
const uint8_t RADIO_CONF_PIPE_ADDR_1[] = {0x13, 0xe3, 0x13, 0x13, 0x34};
const uint8_t RADIO_CONF_PIPE_ADDR_2[] = {0x13, 0xe3, 0x13, 0x13, 0x35};

inline static uint8_t pipe_code(uint8_t id) { return 1 << id; }

inline static uint8_t pipe_code_not(uint8_t id)
{
  return PIPE_CODE_ALL ^ pipe_code(id);
}

static uint8_t connecting_pipe(radio_user_t x, radio_user_t y)
{
  assert(0 <= x && x < CLIENT_NUM);
  assert(0 <= y && y < CLIENT_NUM);

  if ((x == 0 && y == 1) || (x == 1 && y == 0)) {
    return 0;
  } else if ((x == 0 && y == 2) || (x == 2 && y == 0)) {
    return 1;
  } else {
    // x, y = 1, 2
    return 2;
  }
}

static uint8_t related_pipes(radio_user_t x)
{
  uint8_t code = 0;
  for (radio_user_t y = 0; y < CLIENT_NUM - 1; y++) {
    if (y != x) {
      code |= connecting_pipe(x, y);
    }
  }
  return code;
}

static uint8_t *pipe_addr(radio_user_t x, radio_user_t y)
{
  uint8_t pipe_id = connecting_pipe(x, y);
  assert(0 <= pipe_id && pipe_id < PIPE_CNT);

  switch (pipe_id) {
  case 0:
    return RADIO_CONF_PIPE_ADDR_0;
  case 1:
    return RADIO_CONF_PIPE_ADDR_1;
  case 2:
    return RADIO_CONF_PIPE_ADDR_2;
  }
  return NULL;
}

void radio_init_prx(radio_user_t user)
{
  assert(0 <= user && user < CLIENT_NUM);

  uint8_t recv_pipe_bits = related_pipes(user);

  NRF24L01_CE = 0;

  // EN_CRC, CRCO: 2 bytes, PWR_UP, PRX
  NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0b00001111);
  // Enable auto-ack
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, recv_pipe_bits);
  // Enable & configure RX address
  NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, recv_pipe_bits);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8 *)RADIO_CONF_PIPE_ADDR_0,
                     RX_ADR_WIDTH);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P1, (u8 *)RADIO_CONF_PIPE_ADDR_1,
                     RX_ADR_WIDTH);
  NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P2, (u8 *)RADIO_CONF_PIPE_ADDR_2,
                     RX_ADR_WIDTH);
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
  // Enable dynamic payload length
  NRF24L01_Write_Reg(NRF_WRITE_REG + DYNPD, recv_pipe_bits);
  // Enable features: dynamic payload length
  NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, 0b00000100);

  NRF24L01_CE = 1; // CE为高,进入接收模式
}

void radio_init_ptx(radio_user_t user_from, radio_user_t user_to)
{
  assert(0 <= user_from && user_from < CLIENT_NUM);
  assert(0 <= user_to && user_to < CLIENT_NUM);

  uint8_t send_pipe = connecting_pipe(user_from, user_to);
  const uint8_t *send_pipe_addr = pipe_addr(user_from, user_to);
  uint8_t send_pipe_code = pipe_code(send_pipe);

  NRF24L01_CE = 0;

  // EN_CRC, CRCO: 2 bytes, PWR_UP, PRX
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
  NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, send_pipe_addr, TX_ADR_WIDTH);
  NRF24L01_Write_Buf(NRF_WRITE_REG + (RX_ADDR_P0 + send_pipe), send_pipe_addr,
                     RX_ADR_WIDTH);
  // Enable dynamic payload length
  NRF24L01_Write_Reg(NRF_WRITE_REG + DYNPD, send_pipe_code);
  // Enable features: dynamic payload length
  NRF24L01_Write_Reg(NRF_WRITE_REG + FEATURE, 0b00000100);

  NRF24L01_CE = 1; // CE为高,进入接收模式
}
