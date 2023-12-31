#ifndef __REMOTE_H
#define __REMOTE_H
#include "sys.h"
#include "stm32f1xx.h"

#define RDATA   PAin(1)

#define REMOTE_ID 0

extern u8 RmtCnt;

void Remote_Init(void);
u8 Remote_Scan(void);
#endif
