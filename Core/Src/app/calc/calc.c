#include "app/calc.h"

static enum calc_mode mode;

void calc_set_mode(enum calc_mode new_mode) { mode = new_mode; }
