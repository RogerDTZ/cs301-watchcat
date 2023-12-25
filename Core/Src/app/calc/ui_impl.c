#include "lvgl.h"
#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

#include "app/calc/calc.h"
static char content [20] = {0};
static int ptr = 0;
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
	while(ptr!=0){
			ptr--;
			content[ptr]=0;
		}
		calc_set_fml(content);
  lv_obj_t *tabview = lv_event_get_target(e);
  uint16_t tab_index = lv_tabview_get_tab_act(tabview);

  // 0: Common
  // 1: Equation
  // 2: Binary
  calc_set_mode((enum calc_mode)tab_index);
}

void CalcCommClicked0(lv_event_t *e) {content[ptr++] = '0';calc_set_fml(content);}
void CalcCommClicked1(lv_event_t *e) {content[ptr++] = '1';calc_set_fml(content);}
void CalcCommClicked2(lv_event_t *e) {content[ptr++] = '2';calc_set_fml(content);}
void CalcCommClicked3(lv_event_t *e) {content[ptr++] = '3';calc_set_fml(content);}
void CalcCommClicked4(lv_event_t *e) {content[ptr++] = '4';calc_set_fml(content);}
void CalcCommClicked5(lv_event_t *e) {content[ptr++] = '5';calc_set_fml(content);}
void CalcCommClicked6(lv_event_t *e) {content[ptr++] = '6';calc_set_fml(content);}
void CalcCommClicked7(lv_event_t *e) {content[ptr++] = '7';calc_set_fml(content);}
void CalcCommClicked8(lv_event_t *e) {content[ptr++] = '8';calc_set_fml(content);}
void CalcCommClicked9(lv_event_t *e) {content[ptr++] = '9';calc_set_fml(content);}
void CalcCommClickedAdd(lv_event_t *e) {content[ptr++] = '+';calc_set_fml(content);}
void CalcCommClickedSub(lv_event_t *e) {content[ptr++] = '-';calc_set_fml(content);}
void CalcCommClickedMul(lv_event_t *e) {content[ptr++] = '*';calc_set_fml(content);}
void CalcCommClickedDiv(lv_event_t *e) {content[ptr++] = '/';calc_set_fml(content);}
void CalcCommClickedEqual(lv_event_t *e) {calc_set_fml(content);}
void CalcCommClickedClear(lv_event_t *e) {
	while(ptr!=0){
		ptr--;
		content[ptr]=0;
	}
	calc_set_fml(content);
}

void CalcCommClickedLB(lv_event_t *e) {content[ptr++] = '(';calc_set_fml(content);}
void CalcCommClickedRB(lv_event_t *e) {content[ptr++] = ')';calc_set_fml(content);}
void CalcCommClickedPow(lv_event_t *e) {content[ptr++] = '^';calc_set_fml(content);}
void CalcCommClickedBackspace(lv_event_t *e) {content[--ptr] = '\0';calc_set_fml(content);}
void Equation(lv_event_t *e) {content[ptr++] = '=';calc_set_fml(content);}
void CalcEqualConfirm(lv_event_t *e) {calc_set_fml(content);}
void CalcCommClickedX(lv_event_t *e) {content[ptr++] = 'x';calc_set_fml(content);}
void CalcCommClickedY(lv_event_t *e) {content[ptr++] = 'y';calc_set_fml(content);}
void BinaryEqual(lv_event_t *e) {calc_set_fml(content);}
