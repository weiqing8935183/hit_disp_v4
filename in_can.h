#ifndef  __IN_CAN_H
#define  __IN_CAN_H

#include "stm32f10x.h"










#define  CAN_QUEUE_NUM    20                   //定义队列的长度
#define  CAN_QUEUE_IDLE   0
#define  CAN_QUEUE_BUSY   1
typedef struct
{
    CanTxMsg can_tx_str[CAN_QUEUE_NUM];

    uint8_t               head;           //队列头

    uint8_t               tail;           //队列尾


    uint8_t               state;
    
}CANBUS_QUEUE_INFO;



CANBUS_QUEUE_INFO * get_canbus_queue_p(void);
uint8_t  init_canbus_queue(CANBUS_QUEUE_INFO* queue_p);
uint8_t  insert_can_queue_tail(CANBUS_QUEUE_INFO* queue_p, CanTxMsg * cantx_p );
uint8_t  delet_can_queue_head(CANBUS_QUEUE_INFO* queue_p);
CanTxMsg *  get_can_queue_head_p(CANBUS_QUEUE_INFO* queue_p);
uint8_t  start_canbus_queue_send(CANBUS_QUEUE_INFO* queue_p);
uint8_t  can_queue_send_irq(CANBUS_QUEUE_INFO* queue_p);


uint8_t ican_ask_hit_state(uint8_t channel);
uint8_t ican_ask_hit_fdbk(void);
uint8_t ican_ask_bath_volt(void);
uint8_t ican_ask_hit_set(uint8_t channel);
uint8_t ican_ask_history(uint8_t chanel ,uint8_t pre_num);

uint8_t ican_ask_sequence_prd(void);

uint8_t  ican_set_hit_num(uint8_t ch ,uint8_t num);  
uint8_t  ican_set_hit_mode(uint8_t ch);
uint8_t  ican_add_hit_num(uint8_t chanel);
uint8_t  ican_add_odd_hit_num(void);
uint8_t  ican_add_even_hit_num(void);
uint8_t  ican_reversal_hit_hold(uint8_t chanel);
uint8_t  ican_reversal_odd_hit_hold(void);
uint8_t  ican_reversal_even_hit_hold(void);


uint8_t  ican_rec_analyse(CanRxMsg * RxMessage);
#endif
