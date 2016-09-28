#ifndef __HIT_H
#define __HIT_T

#include "stm32f10x.h"

#define  HIT_NUM  6 


#define  HIT_STATE_IDL    (0X00)         //
#define  HIT_STATE_ACT    (0X01)
#define  HIT_STATE_ITV    (0X02)
#define  HIT_STATE_HLD    (0X04)
#define  HIT_STATE_DLY    (0X80)


#define  BLOCK_LIMIT      90
#define  LOCK_LIMIT       90
#define  BIG_LIMIT        90
#define  BREAK_LIMIT      90
#define  LEAKAGE_LIMIT    90

typedef struct  hit_info
{

   uint8_t   serial_num ;        //本结构体的序号，即锤头的序号

   uint8_t   opt;                // 当前的输出值
  
   uint8_t   state;		         //当前打壳的状态 0：空闲    1：打壳动作继电器闭合    2：打壳间隔，继电器松开  4：打壳保持  8 延时打壳 
 
   uint8_t   num;                //需要动作的次数
  
   uint8_t   feed_back; 
 
   int8_t   blk_pro; 
   int8_t   blk_pro_add;           //卡堵附加值 此值用于调整现有的卡堵显示

   int8_t   lkg_pro;               //漏电的概率
    int8_t  lkg_pro_add;               //漏电的概率 
                                           
   int8_t   lock_pro ;                   //卡锤头的概率
   int8_t   lock_pro_add;                //卡锤头的附加概率

   int8_t   big_pro;                     //葫芦头的概率
   int8_t   big_pro_add;                 //葫芦头的附加概率

   int8_t   line_break;                  //断线的概率

   int8_t  high_ratio;
   int8_t  low_ratio;
   int8_t  nor_ratio;

   uint16_t idle_volt;                    //空闲期间的电压
   uint8_t  idle_dev;                     //空闲期间的电压方差

   uint16_t  ad_value;                     //当前锤头的ad原始值 ad_value 是发送过来的原始值 volt是由此值计算出来的
   float     volt;                         //当前锤头的电压值




   uint8_t   rehit_flg;             //重复打壳标志  当卡堵概率小于0时此标志清零  当第一次进入轻度卡堵是自己开始打壳8次
                                    //第一位是进入轻度卡堵的标志 第二位是进入中度卡堵的标志 低三位是进入重度卡堵的标志  第四位是进入高电压的标志
   uint8_t   rehit_num;


   uint8_t   hit_time;               //打壳动作时间
   uint8_t   hit_int_time;           //打壳间隔时间
   uint8_t   hold_time;              //打壳的保持时间

   uint16_t  history_num;

  
  

   
}HIT_INFO ;


HIT_INFO * get_hit_p(uint8_t ch);

uint8_t init_hit_all(HIT_INFO * hit_p);

uint16_t  get_bath_volt(void);

#endif
