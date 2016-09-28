/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：用3.5.0版本建的工程模板。         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
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
 * 函数名：main
 * 描述  : 主函数
 * 输入  ：无
 * 输出  : 无
 */



uint8_t  i=0;
DISP_DYNAMIC_CNL cn;
int main(void)
{
     // for(reset_time=0;reset_time<0xffff;reset_time++);     // 延时一段时间 等到买mcp2515上电
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

      init_timer_list();                            //初始化定时器链表
      init_matrix_data_str(get_matrix_data_p());    //初始化点阵显示数组

      init_key_stru(get_key_p(1));                  //初始化key结构体

      init_disp_item(disp_get_item_p());            //初始化选项灯
      init_hit_all(get_hit_p(1));                   //初始化锤头
      init_gpio();                                  //初始化IO

      out_can_config();                             //外can初始化 40k
      ican_config();                                // 内CAN初始化 1m
   
      init_spi();                                   //点阵屏的spi初始化
      matrix_hal_init();                            //初始化点阵屏  将其复位一次
      spi2_dma_nvic_config();                       //初始化spi的dam中断
      spi2_dma_init();                              //初始化spi的dma
      //
      SysTick_Init();                               //初始化滴答定时器  1ms一次中断
         

   
       RTC_Config();
                                              
       insert_queue_bkg_persent(50);                         //向队列中插入背景色
    
       get_matrix_data_p()->tm_p =creat_timer((void *) get_matrix_data_p(),1 , refresh_matrix_row_hook);  //建立定时器 定时刷新前景色  1ms一次
       get_key_p(0)->tm_p        =creat_timer((void *) get_key_p(0)       ,100 , key_sample_hook);                  //建立定时器 定时 读取按键值  1ms一次


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

       Tim2Config(1000);                       //将定时器2 初始化为1ms一次中断
       TIM2_NVIC_Configuration();             //开定时器2的中断 
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


