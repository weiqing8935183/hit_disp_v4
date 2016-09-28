#include "hit.h"


HIT_INFO  hit[HIT_NUM];
uint16_t bath_volt;

uint16_t  get_bath_volt(void)
{
    return   bath_volt;
}
HIT_INFO * get_hit_p(uint8_t ch)
{

     HIT_INFO * rtn_p;

     if(ch < HIT_NUM)
     {
        rtn_p = &hit[ch];
     }
     else
     {
         rtn_p = 0;
     }

     return rtn_p;
}


uint8_t init_hit_all(HIT_INFO * hit_p)
{
    uint8_t i;
    uint8_t rtn= 0;

    for(i=0;i<HIT_NUM;i++)
    {
       (hit_p+i)->serial_num = i;


       (hit_p+i)->num    = 0;
       (hit_p+i)->opt    = 0;
       (hit_p+i)->feed_back    = 0;

       (hit_p+i)->blk_pro    = 0;
       (hit_p+i)->lock_pro   = 0;
       (hit_p+i)->lkg_pro    = 0;
       (hit_p+i)->big_pro    = 0;
       (hit_p+i)->line_break = 0;

    }

    return rtn ;
}

