#include "lvgl.h"
#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

#include "app/calc/calc.h"

/*=============================================================================
                               UI controllers
 =============================================================================*/

void calc_set_fml(const char *fml_str)
{
  lv_label_set_text(ui_CalcFml, fml_str);
}

/*=============================================================================
                             UI event handlers
 =============================================================================*/

/**
 * Called when user changes the tab. Should alter the calc mode here.
 */
void CalcModeChanged(lv_event_t *e)
{
  lv_obj_t *tabview = lv_event_get_target(e);
  uint16_t tab_index = lv_tabview_get_tab_act(tabview);

  // 0: Common
  // 1: Equation
  // 2: Binary
  calc_set_mode((enum calc_mode)tab_index);
}

void CalcCommClicked0(lv_event_t *e) {}
void CalcCommClicked1(lv_event_t *e) {}
void CalcCommClicked2(lv_event_t *e) {}
void CalcCommClicked3(lv_event_t *e) {}
void CalcCommClicked4(lv_event_t *e) {}
void CalcCommClicked5(lv_event_t *e) {}
void CalcCommClicked6(lv_event_t *e) {}
void CalcCommClicked7(lv_event_t *e) {}
void CalcCommClicked8(lv_event_t *e) {}
void CalcCommClicked9(lv_event_t *e) {}
void CalcCommClickedAdd(lv_event_t *e) {}
void CalcCommClickedSub(lv_event_t *e) {}
void CalcCommClickedMul(lv_event_t *e) {}
void CalcCommClickedDiv(lv_event_t *e) {}
void CalcCommClickedEqual(lv_event_t *e) {}
void CalcCommClickedClear(lv_event_t *e) {}
void CalcCommClickedLB(lv_event_t *e) {}
void CalcCommClickedRB(lv_event_t *e) {}
void CalcCommClickedPow(lv_event_t *e) {}
void CalcCommClickedBackspace(lv_event_t *e) {}