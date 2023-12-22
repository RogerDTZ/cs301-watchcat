#ifndef __CALC_APP_H__
#define __CALC_APP_H__

#include <stdio.h>

typedef enum calc_mode { CALC_MODE_COMM = 0u, CALC_MODE_EQUA, CALC_MODE_BIN };

void calc_set_mode(enum calc_mode new_mode);

void calc_set_fml(const char *fml_str);

#endif /* __CALC_APP_H__ */
