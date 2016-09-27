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


#define pin_matrix_rst        matrix_rst(0)     //rst��  Ϊ��ʱ��λ    Ϊ�ߵ�ʱ����
#define pin_matrix_run        matrix_rst(1)

#define  pin_select_bk        matrix_slb(0)        //seclet background ��  Ϊ��ѡ�񱳾�ɫ
#define  pin_select_fg        matrix_slb(1)

#define  pin_lat_action       matrix_lat(1); matrix_lat(0)  //���涯��  ������lat��Ϊ1  ������Ϊ0 �����涯��

#define  pin_enable_matrix    matrix_en(0)                  //ʹ�ܵ���������
#define  pin_disable_matrix   matrix_en(1)                  //��ֹ��������ʾ

void light_matrix_row(uint8_t temp);
void matrix_hal_init(void);


//////////////////////////////////////////////
/*
������ �����ݲ���
*/
#define MATRIX_NUM    4       //����ģ������� 

typedef struct
{

    uint8_t bk_color[MATRIX_NUM][3];           //MATRIX_NUM ��ʾģ����    �����3�ֱ��ʾ�� �� �� �ı������� ����ÿ����ɫ������ʹ�õ�6λ ���Ϊ63��

    uint8_t bk_color_str[MATRIX_NUM][18];      //ÿ������18λ��ʾ  һ��ģ��ʹ��18*8λ  ��18���ֽ�  
     
    uint8_t fg_color_str[8][MATRIX_NUM][8][3];    //��һ��8 ��ʾ8 ��  MATRIX_NUM ��ʾģ����  8��ʾÿ��ģ���е�8����(��) �����3�ֱ��ʾ�� �� �� �ı������� ����ÿ����ɫ������ʹ�õ�8λ ���Ϊ255


    uint8_t  row_now  ;             //�ϴ�ˢ�µ����� 

    TIMER_STRUCT_INFO * tm_p;
}
MATRIX_DATA_INFO;

MATRIX_DATA_INFO * get_matrix_data_p(void);

uint8_t * get_bsk_str_p(MATRIX_DATA_INFO * p);
uint8_t * get_fg_str_p(MATRIX_DATA_INFO * p);

uint8_t init_matrix_data_str(MATRIX_DATA_INFO * p);
uint8_t extra_bk_color(uint8_t * des_p , uint8_t *src_p);

/*******************************************
���������в���
*********************************************/
#define  BK_TYPE    1
#define  FG_TYPE    0

#define  BK_NUM     72
#define  FG_NUM     96
typedef struct {


             uint16_t   type;                //����ɫ����ǰ��ɫ

             uint16_t   num ;               //��Ҫ���͵�����

             uint8_t *  str_p;                // �ַ�����ָ��




}MATRIX_SEND_MSG_INFO;



#define MATIRX_QUEUE_NUM 10
typedef struct{

               MATRIX_SEND_MSG_INFO   msg_str[MATIRX_QUEUE_NUM];

               uint16_t    head;
               uint16_t    tail;

}MATRIX_SEND_MSG_QUEUE;

/*************************************
���������Ʋ���
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
