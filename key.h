

#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "systick.h"


#define KEY_NUM   4


#define  KEY_HIT2  0
#define  KEY_HIT1  1
#define  KEY_SET   2
#define  KEY_SHT   3

#define KEY_ON     1
#define KEY_OFF    0


#define KEY_DOWN   1
#define KEY_HOLD   2
#define KEY_UP     3
#define KEY_REL    0
typedef struct
{

   uint8_t	 key_now;			 
   uint8_t   key_old;			  
   uint8_t   key_action;		 
   uint16_t  key_hold_time;	      
   
   
   TIMER_STRUCT_INFO * tm_p;	                  

}KEY_INFO;




KEY_INFO  *get_key_p(uint8_t  key_type);

uint8_t init_key_stru(KEY_INFO  * key_p);
uint8_t  key_sample_hook( void * p);
uint16_t  get_key_message(KEY_INFO * key_p);
void clear_key_hold_time(uint8_t chanel);




#endif

