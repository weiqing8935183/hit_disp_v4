#ifndef __INIT_GPIO_H
#define __INIT_GPIO_H

#include "stm32f10x.h"


void init_gpio(void);

void init_addr_gpio(void);
void init_key_gpio(void);
void init_dm163_gpio(void);
void init_led_gpio(void);


uint8_t  get_addr(void);







#endif  
