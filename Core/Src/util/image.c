#include "util/image.h"

#include <math.h>

void get_img_pixel(const lv_img_dsc_t *img_dsc, uint16_t x, uint16_t y,
                   struct pixel_565 *pixel)
{
  uint16_t offset = y * img_dsc->header.w + x;
  pixel->rgb.raw = *(uint16_t *)(img_dsc->data + (offset << 1));
}

void set_img_pixel(const lv_img_dsc_t *img_dsc, uint16_t x, uint16_t y,
                   const struct pixel_565 *pixel)
{
  uint16_t offset = y * img_dsc->header.w + x;
  *(uint16_t *)(img_dsc->data + (offset << 1)) = pixel->rgb.raw;
}
