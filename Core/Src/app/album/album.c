#include "app/album.h"

#include "lvgl.h"
#include "sl_ui/ui.h"

#include "util/image.h"

LV_IMG_DECLARE(sketch_img);

void album_set_image_pixel(uint16_t x, uint16_t y, uint16_t r, uint16_t g,
                           uint16_t b)
{
  struct pixel_565 pixel;
  pixel.rgb.r = r;
  pixel.rgb.g = g;
  pixel.rgb.b = b;
  set_img_pixel(&sketch_img, x, y, &pixel);
}

void album_set_image_pixel_raw(uint16_t x, uint16_t y, uint16_t rgb565)
{
  struct pixel_565 pixel;
  pixel.rgb.raw = rgb565;
  set_img_pixel(&sketch_img, x, y, &pixel);
}

void album_display_image() { lv_img_set_src(ui_AlbumCurrImage, &sketch_img); }

void album_set_label(const char *str)
{
  lv_label_set_text(ui_AlbumPrompt, str);
  lv_obj_set_align(ui_AlbumPrompt, LV_ALIGN_TOP_MID);
}

void open_app_album()
{
  lv_obj_clear_flag(ui_AlbumApp, LV_OBJ_FLAG_HIDDEN);
  album_display_image();
}

void close_app_album() { lv_obj_add_flag(ui_AlbumApp, LV_OBJ_FLAG_HIDDEN); }
