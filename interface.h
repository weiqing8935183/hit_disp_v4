#ifndef   __INTERFACE_H
#define   __INTERFACE_H

#include "stm32f10x.h"

#define  BLOCK_LIMIT      90
#define  LOCK_LIMIT       90
#define  BIG_LIMIT        90
#define  BREAK_LIMIT      90
#define  LEAKAGE_LIMIT    90


uint8_t item_all(void);



void hit_manage(uint8_t ch);


void other_manage(void);

#endif

