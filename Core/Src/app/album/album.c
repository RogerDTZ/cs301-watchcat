#include "app/album.h"

#include "lvgl.h"
#include "sl_ui/ui.h"

void album_set_label(const char *str)
{
  lv_label_set_text(ui_AlbumPrompt, str);
  lv_obj_set_align(ui_AlbumPrompt, LV_ALIGN_TOP_MID);
}

void album_set_displayed_image(const void *img_src)
{
  lv_img_set_src(ui_AlbumCurrImage, img_src);
}

void open_app_album() { lv_obj_clear_flag(ui_AlbumApp, LV_OBJ_FLAG_HIDDEN); }

void close_app_album() { lv_obj_add_flag(ui_AlbumApp, LV_OBJ_FLAG_HIDDEN); }
