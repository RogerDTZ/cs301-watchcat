#ifndef __PERSISTENT_H__
#define __PERSISTENT_H__

#include <stdint.h>

// AT24C02 has 256 bytes of memory

#define PERS_TOUCH_CALIBRATION_ADDR  (0x00)
#define PERS_TOUCH_CALIBRATION_MAGIC (0xbeeffeeb)

typedef struct {
  uint32_t magic;
  float fac_x;
  float fac_y;
  uint16_t center_x;
  uint16_t center_y;
} pers_touch_calibration_t;

#endif /* __PERSISTENT_H__ */
