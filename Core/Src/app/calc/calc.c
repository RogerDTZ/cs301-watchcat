#include "app/calc/calc.h"

static enum calc_mode mode;

void calc_set_mode(enum calc_mode new_mode)
{
  mode = new_mode;

  // [FIXME] demo purpose
  static char demo_fml[] = "Current Mode: 0";
  demo_fml[14] = '0' + (int)mode;
  calc_set_fml(demo_fml);
}
