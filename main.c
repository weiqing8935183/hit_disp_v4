/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ����3.5.0�汾���Ĺ���ģ�塣         
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "systick.h"
#include "dm163.h"
#include "disp.h"
#include "stdio.h"

//#include "init_uart.h"
#include  "init_gpio.h"
#include  "init_tim.h"
#include  "init_spi.h"
#include  "init_can.h"
#include  "init_mcp2515.h"
#include  "key.h"
#include "rtc.h"
#include "interface.h"
#include "hit.h"
#include "in_can.h"

/* 
 * ��������main
 * ����  : ������
 * ����  ����
 * ���  : ��
 */



uint8_t  i=0;
DISP_DYNAMIC_CNL cn;
int main(void)
{
     // for(reset_time=0;reset_time<0xffff;reset_time++);     // ��ʱһ��ʱ�� �ȵ���mcp2515�ϵ�
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

      init_timer_list();                            //��ʼ����ʱ������
      init_matrix_data_str(get_matrix_data_p());    //��ʼ��������ʾ����

      init_key_stru(get_key_p(1));                  //��ʼ��key�ṹ��

      init_disp_item(disp_get_item_p());            //��ʼ��ѡ���
      init_hit_all(get_hit_p(1));                   //��ʼ����ͷ
      init_gpio();                                  //��ʼ��IO

      out_can_config();                             //��can��ʼ�� 40k
      ican_config();                                // ��CAN��ʼ�� 1m
   
      init_spi();                                   //��������spi��ʼ��
      matrix_hal_init();                            //��ʼ��������  ���临λһ��
      spi2_dma_nvic_config();                       //��ʼ��spi��dam�ж�
      spi2_dma_init();                              //��ʼ��spi��dma
      //
      SysTick_Init();                               //��ʼ���δ�ʱ��  1msһ���ж�
         

   
       RTC_Config();
                                              
       insert_queue_bkg_persent(50);                         //������в��뱳��ɫ
    
       get_matrix_data_p()->tm_p =creat_timer((void *) get_matrix_data_p(),1 , refresh_matrix_row_hook);  //������ʱ�� ��ʱˢ��ǰ��ɫ  1msһ��
       get_key_p(0)->tm_p        =creat_timer((void *) get_key_p(0)       ,100 , key_sample_hook);                  //������ʱ�� ��ʱ ��ȡ����ֵ  1msһ��


//            ican_ask_hit_set(1);
//            ican_ask_hit_set(2);
//            ican_ask_hit_set(3);
//            ican_ask_hit_set(4);
//            ican_ask_hit_set(5);
//            ican_ask_hit_set(6);

//
//       cn.dir= DIR_R_L;
//       cn.interval = 100;
//       cn.color = BLUE|RED|GREEN;
//       cn.str_num =32;
//       sprintf((char *)cn.str,"welcome to use hit automation system. It design by CHALCO-ZYY");
//       disp_dynamic_str(&cn) ;
//    

//      while (get_dy_p()->state ==DY_BUSY);
//
//      if(get_hit_p(0)->hit_time == 0)
//      {
//          cn.dir= DIR_D_U;
//          cn.interval = 10;
//          cn.color = RED;
//          cn.str_num =4;
//          sprintf((char *)cn.str,"iCAN");
//          disp_dynamic_str(&cn) ;
//          while(1);
//      }

       Tim2Config(1000);                       //����ʱ��2 ��ʼ��Ϊ1msһ���ж�
       TIM2_NVIC_Configuration();             //����ʱ��2���ж� 
       TIM_Cmd(TIM2,ENABLE);    

  	  while(1)
      {

          switch(disp_get_item_p()->m_item)
          {
            case 1:
                  item_all();
                  break;
            case 2:
                   hit_manage(1);
                   break;
            case 3:
                   hit_manage(2);
                   break;
            case 4:
                   hit_manage(3);
                   break;
            case 5:
                   hit_manage(4);
                   break;
            case 6:
                   hit_manage(5);
                   break;
            case 7:
                   hit_manage(6);
                   break;
            case 8:
                   other_manage();
                   break;
            default:
                  break;
          }
       

          
      }
	
}




/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


