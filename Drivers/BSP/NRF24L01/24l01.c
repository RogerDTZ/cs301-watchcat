#include "24l01.h"
#include "SYSTEM/delay/delay.h"
#include "spi.h"

//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32F103开发板
// NRF24L01驱动代码
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 创建日期:2019/9/19
// 版本：V1.0
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

static u8 SPI1_ReadWriteByte(u8 data_tx)
{
  u8 data_rx;
  HAL_SPI_TransmitReceive(&hspi1, &data_tx, &data_rx, 1, 1000);
  return data_rx;
}

u8 NRF24L01_Write_Reg(u8 reg, u8 value)
{
  u8 status;
  NRF24L01_CSN = 0;                 // 使能SPI传输
  status = SPI1_ReadWriteByte(reg); // 发送寄存器号
  SPI1_ReadWriteByte(value);        // 写入寄存器的值
  NRF24L01_CSN = 1;                 // 禁止SPI传输
  return status;                    // 返回状态值
}

u8 NRF24L01_Read_Reg(u8 reg)
{
  u8 reg_val;
  NRF24L01_CSN = 0;                   // 使能SPI传输
  SPI1_ReadWriteByte(reg);            // 发送寄存器号
  reg_val = SPI1_ReadWriteByte(0XFF); // 读取寄存器内容
  NRF24L01_CSN = 1;                   // 禁止SPI传输
  return reg_val;                     // 返回状态值
}

u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status, u8_ctr;
  NRF24L01_CSN = 0;                 // 使能SPI传输
  status = SPI1_ReadWriteByte(reg); // 发送寄存器值(位置),并读取状态值
  for (u8_ctr = 0; u8_ctr < len; u8_ctr++) {
    pBuf[u8_ctr] = SPI1_ReadWriteByte(0XFF); // 读出数据
  }
  NRF24L01_CSN = 1; // 关闭SPI传输
  return status;    // 返回读到的状态值
}

u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status, u8_ctr;
  NRF24L01_CSN = 0;                 // 使能SPI传输
  status = SPI1_ReadWriteByte(reg); // 发送寄存器值(位置),并读取状态值
  for (u8_ctr = 0; u8_ctr < len; u8_ctr++) {
    SPI1_ReadWriteByte(*pBuf++); // 写入数据
  }
  NRF24L01_CSN = 1; // 关闭SPI传输
  return status;    // 返回读到的状态值
}

void NRF24L01_Init(void)
{
  NRF24L01_CE = 0;  // 使能24L01
  NRF24L01_CSN = 1; // SPI片选取消
}

u8 NRF24L01_Check(void)
{
  u8 i;
  u8 buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
  NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, buf, 5);
  NRF24L01_Read_Buf(TX_ADDR, buf, 5);
  for (i = 0; i < 5; i++) {
    if (buf[i] != 0XA5) {
      return 1;
    }
  }
  return 0;
}

u8 NRF24L01_TxPacket(u8 *tx_buf)
{
  u8 status;

  NRF24L01_CE = 0;
  NRF24L01_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);
  NRF24L01_CE = 1;
  while (NRF24L01_IRQ != 0)
    ;

  status = NRF24L01_Read_Reg(STATUS);
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, status);

  // Failed due to reaching maximum retry time
  if (status & MAX_TX) {
    NRF24L01_Write_Reg(FLUSH_TX, 0xff);
    return MAX_TX;
  }
  // Success
  if (status & TX_OK) {
    return TX_OK;
  }
  // Failed due to unknown reason
  return 0xFF;
}

u8 NRF24L01_RxPacket(u8 *rx_buf, u8 *rx_pipe)
{
  u8 status;

  status = NRF24L01_Read_Reg(STATUS);
  NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, status);
  if (status & RX_OK) {
    if (rx_pipe) {
      *rx_pipe = (status & STATUS_RX_P_NO_MASK) >> 1;
    }
    NRF24L01_Read_Buf(RD_RX_PLOAD, rx_buf, RX_PLOAD_WIDTH);
    NRF24L01_Write_Reg(FLUSH_RX, 0xFF);
    return 0;
  }
  return 1;
}
