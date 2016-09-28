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

   uint8_t   serial_num ;        //���ṹ�����ţ�����ͷ�����

   uint8_t   opt;                // ��ǰ�����ֵ
  
   uint8_t   state;		         //��ǰ��ǵ�״̬ 0������    1����Ƕ����̵����պ�    2����Ǽ�����̵����ɿ�  4����Ǳ���  8 ��ʱ��� 
 
   uint8_t   num;                //��Ҫ�����Ĵ���
  
   uint8_t   feed_back; 
 
   int8_t   blk_pro; 
   int8_t   blk_pro_add;           //���¸���ֵ ��ֵ���ڵ������еĿ�����ʾ

   int8_t   lkg_pro;               //©��ĸ���
    int8_t  lkg_pro_add;               //©��ĸ��� 
                                           
   int8_t   lock_pro ;                   //����ͷ�ĸ���
   int8_t   lock_pro_add;                //����ͷ�ĸ��Ӹ���

   int8_t   big_pro;                     //��«ͷ�ĸ���
   int8_t   big_pro_add;                 //��«ͷ�ĸ��Ӹ���

   int8_t   line_break;                  //���ߵĸ���

   int8_t  high_ratio;
   int8_t  low_ratio;
   int8_t  nor_ratio;

   uint16_t idle_volt;                    //�����ڼ�ĵ�ѹ
   uint8_t  idle_dev;                     //�����ڼ�ĵ�ѹ����

   uint16_t  ad_value;                     //��ǰ��ͷ��adԭʼֵ ad_value �Ƿ��͹�����ԭʼֵ volt���ɴ�ֵ���������
   float     volt;                         //��ǰ��ͷ�ĵ�ѹֵ




   uint8_t   rehit_flg;             //�ظ���Ǳ�־  �����¸���С��0ʱ�˱�־����  ����һ�ν�����ȿ������Լ���ʼ���8��
                                    //��һλ�ǽ�����ȿ��µı�־ �ڶ�λ�ǽ����жȿ��µı�־ ����λ�ǽ����ضȿ��µı�־  ����λ�ǽ���ߵ�ѹ�ı�־
   uint8_t   rehit_num;


   uint8_t   hit_time;               //��Ƕ���ʱ��
   uint8_t   hit_int_time;           //��Ǽ��ʱ��
   uint8_t   hold_time;              //��ǵı���ʱ��

   uint16_t  history_num;

  
  

   
}HIT_INFO ;


HIT_INFO * get_hit_p(uint8_t ch);

uint8_t init_hit_all(HIT_INFO * hit_p);

uint16_t  get_bath_volt(void);

#endif
