#ifndef   __INTERFACE_H
#define   __INTERFACE_H

#include "stm32f10x.h"


#define  DISP_BLOCK      80
#define  DISP_LOCK       80
#define  DISP_BIG        80
#define  DISP_BREAK      80
#define  DISP_LEAKAGE    80


uint8_t item_all(void);



void hit_manage(uint8_t ch);


void other_manage(void);

#endif

