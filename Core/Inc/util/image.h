#ifndef __IMG_UTIL_H__
#define __IMG_UTIL_H__

#include <stdint.h>

#include "lvgl.h"

struct pixel_565 {
  union {
    struct {
      uint16_t b : 5;
      uint16_t g : 6;
      uint16_t r : 5;
    };
    uint16_t raw;
  } rgb;
};

void get_img_pixel(const lv_img_dsc_t *img_dsc, uint16_t x, uint16_t y,
                   struct pixel_565 *pixel);

void set_img_pixel(const lv_img_dsc_t *img_dsc, uint16_t x, uint16_t y,
                   const struct pixel_565 *pixel);

#endif /* __IMG_UTIL_H__ */
