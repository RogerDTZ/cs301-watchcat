#include "radio/radio.h"

#include <stdio.h>

#include "BSP/NRF24L01/24l01.h"
#include "sl_ui/ui.h"

#include "app/calc/calc.h"

static int local_id;

void radio_test_set_mode(int id)
{
  local_id = id;

  static char str[16];
  radio_init_prx(id);
  sprintf(str, "I am %d", id);
  calc_set_fml(str);
}

void radio_test_send(int target)
{
  if (target == local_id) {
    return;
  }
  radio_init_ptx(local_id, target);

  const char payload[33] = "Greetings!";
  int ret = NRF24L01_TxPacket((uint8_t *)payload);
  char res[32];
  sprintf(res, "Sent to %d, res: %d", target, ret);
  lv_label_set_text(ui_CalcFml, res);

  radio_init_prx(local_id);
}
