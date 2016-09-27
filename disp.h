#ifndef __DISP_H

#define __DISP_H

#include "stm32f10x.h"
#include "systick.h"
#include "dm163.h"


//定义颜色的宏    此部分可增加 
#define  LBLUE  0x0000ffff
#define  YELLOW 0x00ffff00
#define  PURPLE 0x00ff00ff
#define  WHITE  0x00ffffff
#define  RED    0X00ff0000
#define  GREEN  0X0000FF00
#define  BLUE   0X000000ff

typedef uint8_t fg[8][MATRIX_NUM][8][3] ;


#define  MAX_CHAR_NUM  131

#define  HIT_CHAR   7
#define  R_HIT_CHAR 28
#define  S_BLK_CHAR 124
#define  M_BLK_CHAR 125
#define  L_BLK_CHAR 126
#define  LKG_CHAR   127
#define  BIG_CHAR   128
#define  BRK_CHAR   129
#define  CANID_CAHR 130
#define  VRN_CHAR   131


//静态显示函数   ，此部分不管动态显示的什么 直接向显示缓存中写入东西
uint8_t  disp_point(fg des_p,uint8_t row,uint8_t list,uint32_t color );
uint8_t  disp_char(fg des_p,uint8_t num,uint32_t color ,uint8_t ch );
uint8_t  disp_or_char(fg des_p,uint8_t num,uint32_t color ,uint8_t ch);
uint8_t  disp_str(fg des_p,uint32_t color ,uint8_t * str );


uint8_t disp_int(fg p,uint32_t color ,int16_t num);
uint8_t disp_float(fg p,uint32_t color ,float num);




//动态显示结构体 
#define DY_BUSY       1
#define DY_IDLE       0

#define DY_CHAR       0
#define DY_DATA       1
typedef struct
{
   uint8_t     type;
   uint8_t     state;
   uint8_t     dir;
   uint16_t    have_done;
   uint16_t    need_done;
   uint32_t    interval;           //间隔 以毫秒为单位
   uint32_t    color;             //颜色
   uint8_t     str_num;            //欲写入字节的个数
   uint8_t     str[128];
   fg          data;
   TIMER_STRUCT_INFO *tm_p;

}DISP_DYNAMIC_CNL;


DISP_DYNAMIC_CNL * get_dy_p(void);

uint8_t disp_dynamic_str_hook(void * p);
uint8_t disp_dynamic_str(DISP_DYNAMIC_CNL *cnl_p);

uint8_t disp_dynamic_data_hook(void * p);
uint8_t disp_dynamic_data(void);










typedef struct  
{
   uint8_t   m_item;
   uint8_t   s_item;


}DISP_ITEM_INFO;



uint8_t  init_disp_item(DISP_ITEM_INFO *p);
DISP_ITEM_INFO * disp_get_item_p(void);
uint8_t disp_m_item(void);




#endif 

