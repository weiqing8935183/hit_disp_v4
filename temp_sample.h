#ifndef __TEMP_SAMPLE_H
#define __TEMP_SAMPLE_H

#include "stm32f10x.h"

#define  DEVICE_NUM   5             //定义前端设备的个数

#define  INQUIRE_INTERVAL  50000    //定义询问的间隔    1 = 100us


#define  REC_BUSY         1
#define  REC_SUCCESS      0
#define  REC_ERR0R        2
#define  REC_INQUIRE      3

typedef struct
{
   uint8_t  state;
   uint8_t  rec_str[60];
   uint8_t  rec_num;
}RECEIVE_INFO_STRUCT;



#define DEVICE_ERR  1
#define DEVICE_NOR  0
typedef  struct 
{
    uint8_t  state;

    uint8_t  addr;

    RECEIVE_INFO_STRUCT  rec_stru;

    int16_t  device_temp;

    int16_t temp_str[24];

    int8_t  posi_str[24];


}SAMPLE_INFO_STRUCT;



SAMPLE_INFO_STRUCT * get_sample_p(void);
uint8_t  init_sample_str(SAMPLE_INFO_STRUCT * sm_p);
uint8_t  sample_inquire_prd_hook(void * p);

uint8_t  rec_stru_crc(RECEIVE_INFO_STRUCT * rec_p);

uint8_t  clear_data(SAMPLE_INFO_STRUCT * sm_p);
uint8_t  sample_divice_timer_hook(void * sm_p);



typedef struct 
{
    int16_t   temp;                 // 此点的温度

    uint16_t  devive_addr;          //此点的设备地址  高8位为设备地址  低八位为设备中的采样点地址
   
}TEMP_NODE_INFO;


typedef struct 
{
     TEMP_NODE_INFO   temp_a_str[56];
     TEMP_NODE_INFO   temp_b_str[56];
}BATHTEMP_INFO;
#endif


