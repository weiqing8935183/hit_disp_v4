#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

#define  TIMER_NUM  40


typedef struct  timer_struct_info
{
    void *   timer_para;

    uint32_t  dec_num;                       //�ݼ�ֵ ÿ��ʱ���жϽ���ֵ�ݼ� ��Ϊ0����

    uint32_t  set_num;

    uint8_t  (*over_fun)(void * );           //����ʱ��ʱʱ���õĺ���

    struct timer_struct_info * next_p;       //ָ����һ����ʱ���ṹ��


  
}TIMER_STRUCT_INFO;



typedef struct
{

    TIMER_STRUCT_INFO *  head_p;
    uint8_t              list_num;
   
}TIMER_LIST_INFO;





void SysTick_Init(void);
void init_timer_list(void);
TIMER_STRUCT_INFO * creat_timer(void * flag,uint32_t  interval ,uint8_t (*fun)(void * ));
uint8_t  refresh_timer(TIMER_STRUCT_INFO *tm_p);
uint8_t  change_timer(TIMER_STRUCT_INFO *tm_p,uint32_t interval);
uint8_t  destory_timer(TIMER_STRUCT_INFO *tm_p);
uint8_t timer_prd_irq(void);

#endif /* __SYSTICK_H */
