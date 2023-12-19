#include "lvgl.h"

#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

static int counter = 0;

void home_demo_button_clicked(lv_event_t *e)
{
  lv_label_set_text_fmt(ui_DemoButtonLabel, "Count: %d", ++counter);
}
