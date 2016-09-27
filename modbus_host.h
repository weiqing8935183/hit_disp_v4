#ifndef   __MODBUS_HOST_H
#define   __MODBUS_HOST_H

#include "stm32f10x.h"
#include "systick.h"
#include "temp_sample.h"
//


//发送数据帧的结构体

typedef struct 
{
  

    uint8_t  send_str[16];
    uint8_t  need_send_num;
    uint8_t  have_send_num;

    RECEIVE_INFO_STRUCT  *rec_stru_p;


}MODBUS_FRAME_INOF;



#define  QUEUE_NUM 10                    //定义队列的长度

typedef struct
{
    MODBUS_FRAME_INOF     frame_str[QUEUE_NUM];

    uint8_t               head;           //队列头

    uint8_t               tail;           //队列尾
    
}MODBUS_QUEUE_INFO;


MODBUS_QUEUE_INFO * get_modbus_queue_p(void);
uint8_t  init_modbus_queue(MODBUS_QUEUE_INFO* queue_p);
uint8_t  insert_mod_queue_tail(MODBUS_QUEUE_INFO* queue_p, MODBUS_FRAME_INOF * modbus_p );
uint8_t  delet_mod_queue_head(MODBUS_QUEUE_INFO* queue_p);
MODBUS_FRAME_INOF *  get_mod_queue_head_p(MODBUS_QUEUE_INFO* queue_p);



/////////////////////////////////////////////////////////////////////////////
/*控制状态结构体*/
////////////////////////////////////////////////////////////////////////////

#define  MB_IDLE             0

//#define  MB_WAIT_SEND        1

#define  MB_SEND             2

#define  MB_SEND_END         3

#define  MB_WAIT_REC         4

#define  MB_REC              5

//#define  MB_RED_END          6

#define  MB_FRM_ITV          7      //帧和帧之间的间隔

#define  MB_ERR            0X10

typedef struct 
{

   
//   uint8_t  mode ;                         //控制器模式  0 为host       1 为slave
//   uint8_t  baudrate ;                     //波特率

   uint8_t  state;                         // 控制器当前状态
   
   MODBUS_FRAME_INOF *mf_p;                //控制器当前操作指向的数据帧 modbus fram p

   TIMER_STRUCT_INFO *tm_p;                 //当前所指向的定时器指针

}MODBUS_CONTRAL_INFO;

#define  modbus_hal_set_send()       GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define  modbus_hal_set_receive()    GPIO_ResetBits(GPIOB,GPIO_Pin_13)


MODBUS_CONTRAL_INFO * modbus_get_cm_p( void);
uint8_t  start_modbus_queue_send(MODBUS_QUEUE_INFO* queue_p,MODBUS_CONTRAL_INFO *mc_p);
uint8_t  modbus_send_irq(MODBUS_CONTRAL_INFO * cm_p);
uint8_t  modbus_receive_irq(MODBUS_CONTRAL_INFO * cm_p);


uint8_t  modbus_timeout_hook(void * p);
uint8_t  modbus_inquire_ped_hook(void * p);

unsigned short CRC16(unsigned char * puchMsg,unsigned short usDataLen) ;
#endif

