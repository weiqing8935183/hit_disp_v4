#include "key.h"
#include "systick.h"


KEY_INFO key_str[KEY_NUM];


KEY_INFO  *get_key_p(uint8_t  key_type)
{
    KEY_INFO *rtn_p;
    if(key_type >= KEY_NUM)
    {
            rtn_p = 0;
    }
    else
    {
          rtn_p =  &key_str[key_type];

    }

    return rtn_p ;
}
uint8_t init_key_stru(KEY_INFO  * key_p)
{
    uint8_t rtn = 0;
    uint8_t  i;
    for(i=0 ; i< KEY_NUM; i++)
    {
       key_p->key_now = KEY_OFF;
       key_p->key_old = KEY_OFF;
       key_p->key_action = KEY_REL;
       key_p->key_hold_time  = 0;
    }

    return rtn ;
}
uint8_t  get_key_state(uint8_t *key)
{
     uint8_t rtn=0;

     *key =  (~(GPIO_ReadInputData(GPIOE)>>8))&0X0F;

     return rtn ;
}
/*******************************************
按键采样函数 
此函数在定时中断中调用

*******************************************/
uint8_t  key_sample_hook( void * p)
{
    uint8_t tempkey;
    uint8_t rtn=0 ;
    uint8_t  i;
    KEY_INFO * key_p;
    key_p = p;
    get_key_state(&tempkey);
    for(i=0;i<KEY_NUM;i++)
    {
          (key_p+i)->key_old=(key_p+i)->key_now;
		  if(tempkey&(1<<i))
	      {
			 (key_p+i)->key_now=KEY_ON;
		  }
		  else
	      {
			 (key_p+i)->key_now=KEY_OFF;
		  }
			 
		  if(((key_p+i)->key_old==0)&&((key_p+i)->key_now==1))       //??°′?üμ??ˉ×÷±ê???3?μ  °′?ü°′??±ê??
		  {
			      (key_p+i)->key_action=KEY_DOWN;
		  }
	      else if(((key_p+i)->key_old==1)&&((key_p+i)->key_now==1))	  //°′?ü?y?ú±￡3?
		  {
				  (key_p+i)->key_action=KEY_HOLD;
				  (key_p+i)->key_hold_time += 10;
		  }
		 else if(((key_p+i)->key_old==1)&&((key_p+i)->key_now==0))   //°′?ü?é?a±ê??￡?
	     {
				  (key_p+i)->key_action=KEY_UP;
				  (key_p+i)->key_hold_time=0;
	     }
		 else if(((key_p+i)->key_old==0)&&((key_p+i)->key_now==0))	  //°′?ü±?êí·?
	     {
				  (key_p+i)->key_action=KEY_REL;
		 }

    }

//   if(creat_timer((void *) get_key_p(0) ,10 , key_sample_hook) ==0 )
//   {
//      rtn =1;
//   }
         refresh_timer(get_key_p(0)->tm_p); 
   return rtn;
}

/***************************************
读取按键的键值
每个按键占两位  
按键保持必须在按下一秒以后才会返回按键保持 

1 按下 
2 保持
3 抬起
0 空闲  即松开
****************************************/

uint16_t  get_key_message(KEY_INFO * key_p)
{
    uint16_t rtn=0;
    uint8_t  i;
   
    for(i=0;i<KEY_NUM;i++)
    {
       if(((key_p+i)->key_action!=KEY_HOLD)&&((key_p+i)->key_action!=KEY_REL))
       {
        rtn|=((key_p+i)->key_action<<(i*2));
        (key_p+i)->key_action=0;
       }
       else if( ((key_p+i)->key_action==KEY_HOLD)&&((key_p+i)->key_hold_time>1000) )
       {
            rtn|=((key_p+i)->key_action<<(i*2));
            
            (key_p+i)->key_hold_time=1000;
       }
       
    }

    return rtn;
}

void clear_key_hold_time(uint8_t chanel)
{
    key_str[chanel].key_hold_time=0;
}

