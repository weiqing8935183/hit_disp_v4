#ifndef __DM163_H
#define __DM163_H

#include "stm32f10x.h"
#include "systick.h"



/*****************************
define the IO
*****************************/
#define RST_BIT ((uint16_t)0x0400)   //PB10
#define LAT_BIT ((uint16_t)0x0800)   //PB11
#define SLB_BIT ((uint16_t)0x1000)   //PB12
#define EN_BIT  ((uint16_t)0x4000)   //PB14


void matrix_rst(unsigned char temp);
void matrix_lat(unsigned char temp);
void matrix_slb(unsigned char temp);
void matrix_en(unsigned char temp);


#define pin_matrix_rst        matrix_rst(0)     //rst脚  为低时候复位    为高的时候工作
#define pin_matrix_run        matrix_rst(1)

#define  pin_select_bk        matrix_slb(0)        //seclet background 脚  为低选择背景色
#define  pin_select_fg        matrix_slb(1)

#define  pin_lat_action       matrix_lat(1); matrix_lat(0)  //锁存动作  先设置lat脚为1  再设置为0 即锁存动作

#define  pin_enable_matrix    matrix_en(0)                  //使能点阵屏锁存
#define  pin_disable_matrix   matrix_en(1)                  //禁止点阵屏显示

void light_matrix_row(uint8_t temp);
void matrix_hal_init(void);


//////////////////////////////////////////////
/*
点阵屏 屏数据部分
*/
#define MATRIX_NUM    4       //点阵模块的数量 

typedef struct
{

    uint8_t bk_color[MATRIX_NUM][3];           //MATRIX_NUM 表示模块数    后面的3分别表示蓝 绿 红 的背景数据 其中每个颜色的数据使用低6位 最大为63；

    uint8_t bk_color_str[MATRIX_NUM][18];      //每个点用18位表示  一个模块使用18*8位  即18个字节  
     
    uint8_t fg_color_str[8][MATRIX_NUM][8][3];    //第一个8 表示8 行  MATRIX_NUM 表示模块数  8表示每个模块中的8个点(列) 后面的3分别表示蓝 绿 红 的背景数据 其中每个颜色的数据使用低8位 最大为255


    uint8_t  row_now  ;             //上次刷新的行数 

    TIMER_STRUCT_INFO * tm_p;
}
MATRIX_DATA_INFO;

MATRIX_DATA_INFO * get_matrix_data_p(void);

uint8_t * get_bsk_str_p(MATRIX_DATA_INFO * p);
uint8_t * get_fg_str_p(MATRIX_DATA_INFO * p);

uint8_t init_matrix_data_str(MATRIX_DATA_INFO * p);
uint8_t extra_bk_color(uint8_t * des_p , uint8_t *src_p);

/*******************************************
点阵屏队列部分
*********************************************/
#define  BK_TYPE    1
#define  FG_TYPE    0

#define  BK_NUM     72
#define  FG_NUM     96
typedef struct {


             uint16_t   type;                //背景色还是前景色

             uint16_t   num ;               //需要发送的字数

             uint8_t *  str_p;                // 字符串的指针




}MATRIX_SEND_MSG_INFO;



#define MATIRX_QUEUE_NUM 10
typedef struct{

               MATRIX_SEND_MSG_INFO   msg_str[MATIRX_QUEUE_NUM];

               uint16_t    head;
               uint16_t    tail;

}MATRIX_SEND_MSG_QUEUE;

/*************************************
点阵屏控制部分
***************************************/
#define MATRIX_CRL_BUSY  1
#define MATRIX_CRL_IDLE  0
typedef struct 
{
   uint8_t  state;

   MATRIX_SEND_MSG_INFO *    msg_p;


}MATRIX_CONTRAL_INFO;
MATRIX_CONTRAL_INFO *get_matrix_crl_p(void);
uint8_t  end_matrix_row_send_irp(MATRIX_CONTRAL_INFO * crl_p);
uint8_t  insert_queue_bkg(uint32_t color);
uint8_t  insert_queue_bkg_persent(uint8_t per);
uint8_t  insert_queue_fg(uint8_t row);

uint8_t  refresh_matrix_row_hook(void * p);





//


#define  DIR_U_D   0
#define  DIR_D_U   1
#define  DIR_R_L   2
#define  DIR_L_R   3


#endif  
