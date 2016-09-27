#include  "stdio.h"
#include "interface.h"
#include "disp.h"
#include "key.h"
#include "in_can.h"
#include "systick.h"
#include "hit.h"
#include  "DM163.h"
#include "init_gpio.h"
#include "rtc.h"


static TIMER_STRUCT_INFO * tm_odd_p=0,*tm_even_p=0;

uint8_t  refresh_hit_hook (void * p)
{
  
    uint8_t  rtn,odd_even;
    uint8_t i,j;

    odd_even = (uint32_t)p;

    if(odd_even %2 != 0)                //表示是奇数定时器溢出
    {
       change_timer(tm_odd_p,4000);           //改变定时器的定时时间  并刷新他
    }
    else                              //表示是偶数定时器溢出
    {
       change_timer(tm_even_p,4000);
    }

    if(get_dy_p()->state == DY_IDLE)
    {
       get_dy_p()->type     = DY_DATA;
       get_dy_p()->interval = 100;

       if(odd_even %2 != 0)                //表示是奇数数定时器溢出
       {
          get_dy_p()->dir= DIR_D_U;
          disp_str(get_dy_p()->data,GREEN,"2   ");
          i= 0;


       }
       else                              //表示是偶数定时器溢出
       {
          get_dy_p()->dir= DIR_U_D;
          disp_str(get_dy_p()->data,GREEN,"1   "); 
          i=1;

       }

          i =  odd_even%2;
          for(;i<HIT_NUM;i+=2)
          {

              if(get_hit_p(i)->num != 0)                    //显示剩余的打壳次数
              {
                  for(j=0;j<get_hit_p(i)->num;j++)
                  {
                      disp_point(get_dy_p()->data,0,(i/2+1)*8+j,GREEN );
                  }
              }
              if(get_hit_p(i)->opt == 1)
              {
                   disp_or_char(get_dy_p()->data,i/2+1,GREEN,HIT_CHAR);
              }

              if(get_hit_p(i)->feed_back == 1)
              { 
                   disp_or_char(get_dy_p()->data,i/2+1,BLUE,HIT_CHAR);
              }

              if(get_hit_p(i)->blk_pro > BLOCK_LIMIT)         //堵
              { 
                    disp_or_char(get_dy_p()->data,i/2+1,RED,M_BLK_CHAR);
              }

              if(get_hit_p(i)->lock_pro > LOCK_LIMIT)        //卡
              { 
                    disp_or_char(get_dy_p()->data,i/2+1,RED,HIT_CHAR);
              }

              if(get_hit_p(i)->big_pro > BIG_LIMIT)          //大葫芦头
              { 
                    disp_or_char(get_dy_p()->data,i/2+1,RED,BIG_CHAR);
              }

              if(get_hit_p(i)->lkg_pro > LEAKAGE_LIMIT)          //漏电
              { 
                    disp_or_char(get_dy_p()->data,i/2+1,RED,LKG_CHAR);
              }

              if(get_hit_p(i)->line_break > BREAK_LIMIT)          //断线
              { 
                    disp_or_char(get_dy_p()->data,i/2+1,RED,BRK_CHAR);
              }
          }
       disp_dynamic_data(); 
    }
    else
    {
       rtn =1;
    }

  
  return rtn;
     
}
uint8_t  refresh_hit_num_hook (void * p)
{
          uint8_t  rtn,odd_even;
    uint8_t i;

    odd_even = (uint32_t)p;
    
    if(odd_even %2 != 0)                //表示是奇数数定时器溢出
    {
       change_timer(tm_odd_p,4000);           //改变定时器的定时时间  并刷新他
    }
    else                              //表示是偶数定时器溢出
    {
       change_timer(tm_even_p,4000);
    }

    if(get_dy_p()->state == DY_IDLE)
    {
       get_dy_p()->type     = DY_DATA;
       get_dy_p()->interval = 100;

       if(odd_even %2 != 0)                //表示是偶数定时器溢出
       {
          get_dy_p()->dir= DIR_D_U;
          disp_str(get_dy_p()->data,GREEN,"1   ");
          

         i=1;
       }
       else                              //表示是奇数定时器溢出
       {
          get_dy_p()->dir= DIR_U_D;
          disp_str(get_dy_p()->data,GREEN,"2   "); 
         
         i=0;
       }

           i =  odd_even;
          for(;i<6;i+=2)
          {
              if(get_hit_p(i)->state != HIT_STATE_HLD)
              {
                   disp_or_char(get_dy_p()->data,i/2+1,GREEN,get_hit_p(i*2+odd_even)->num+0x30);
                   if(get_hit_p(i)->feed_back ==1)
                   {
                        disp_or_char(get_dy_p()->data,i/2+1,BLUE,get_hit_p(i/2*2+1)->num+0x30);
                   }
              }
              else
              {
                   disp_or_char(get_dy_p()->data,i/2+1,GREEN,'H');
                   if(get_hit_p(i/2*2+1)->feed_back ==1)
                   {
                        disp_or_char(get_dy_p()->data,i/2+1,BLUE,'H');
                   }
              }

          }
       disp_dynamic_data(); 

     }  
     else
     {
        rtn = 1;
     }  


     return rtn ;
}

uint8_t  refresh_bat_hvolt_hook (void * p)
{
  
    uint8_t  rtn=0;
   

    disp_float(get_matrix_data_p()->fg_color_str,BLUE,get_bath_volt()*2.5*5.7/4096);
    change_timer(tm_odd_p,100);
  
    return rtn;
     
}
/*************************************总览显示函数**************************************************/





uint8_t item_all(void)
{
    uint8_t rtn ; 
//    uint8_t str[8];
    uint16_t key_msg ;
    
     disp_get_item_p()->m_item = disp_get_item_p()->s_item =1;
     while(get_dy_p()->state == DY_BUSY)
     {
         disp_dynamic_data(); 
     }
     tm_odd_p  = creat_timer((void *)1,  100 , refresh_hit_hook);           //建立定时器 定时刷新前景色  1ms一次
     tm_even_p = creat_timer((void *)0, 2100 , refresh_hit_hook);             //建立定时器 定时刷新前景色  1ms一次

     while((disp_get_item_p()->m_item == 1)&&(disp_get_item_p()->s_item == 1))
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                             ican_add_odd_hit_num();
                                             break;
                case KEY_DOWN<<(KEY_HIT2*2) :
                                             ican_add_even_hit_num();
                                             break;

                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                            disp_get_item_p()->s_item ++;
                                             break;

               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     destory_timer(tm_even_p);
     tm_even_p = tm_odd_p = 0;





    /**************************************动态显示锤头状态 数值*****************************************************/
     while(get_dy_p()->state == DY_BUSY)
     {
         disp_dynamic_data(); 
     }
     if((disp_get_item_p()->m_item == 1)&&(disp_get_item_p()->s_item == 2))
     {
         tm_odd_p  = creat_timer((void *)1,  100 , refresh_hit_num_hook);           //建立定时器 定时刷新前景色  1ms一次
         tm_even_p = creat_timer((void *)2, 2100 , refresh_hit_num_hook);             //建立定时器 定时刷新前景色  1ms一次
     }
     while((disp_get_item_p()->m_item == 1)&&(disp_get_item_p()->s_item == 2))
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                             ican_add_odd_hit_num();
                                             break;
                case KEY_DOWN<<(KEY_HIT2*2) :
                                             ican_add_even_hit_num();
                                             break;

                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                            disp_get_item_p()->s_item ++;
                                             break;

               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     destory_timer(tm_even_p);
     tm_even_p = tm_odd_p = 0;

       

 /***************************************显示槽压值*********************************************************************/


    if((disp_get_item_p()->m_item == 1)&&(disp_get_item_p()->s_item == 3))     //动态切换过程 电压值由下至上
     {
        while(get_dy_p()->state == DY_BUSY)
        {
             disp_dynamic_data(); 
        }       
      disp_float(get_dy_p()->data,BLUE,get_bath_volt()*2.5*5.7/4096);
      get_dy_p()->type =  DY_DATA;
      get_dy_p()->dir= DIR_D_U;
      get_dy_p()->interval = 50;
      disp_dynamic_data();
      while(get_dy_p()->state == DY_BUSY);
       tm_odd_p  = creat_timer((void *)1,   1000 , refresh_bat_hvolt_hook);           //建立定时器 定时刷新前景色  1ms一次
     }



     while((disp_get_item_p()->m_item == 1)&&(disp_get_item_p()->s_item == 3))
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                             ican_add_odd_hit_num();
                                             break;
                case KEY_DOWN<<(KEY_HIT2*2) :
                                             ican_add_even_hit_num();
                                             break;

                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                            disp_get_item_p()->s_item ++;
                                             break;

               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;






   /**************************************调整背景亮度*********************************************************/
     while(get_dy_p()->state == DY_BUSY);           //等待运动结束
//     if((disp_get_item_p()->m_item == 1)&&(disp_get_item_p()->s_item == 4))
//     {
//       rtn = get_matrix_data_p()->bk_color[0][0];
//
//      sprintf((char *)str,"%4d",rtn);          //此部分为切入动画
//      disp_str(get_dy_p()->data,WHITE,str);
//      get_dy_p()->type =  DY_DATA;
//      get_dy_p()->dir= DIR_D_U;
//      get_dy_p()->interval = 50;
//      disp_dynamic_data();
//      while(get_dy_p()->state == DY_BUSY);
//     }
//     while((disp_get_item_p()->m_item == 1)&&(disp_get_item_p()->s_item == 4))
//     {
//         key_msg = get_key_message(get_key_p(0));
//
//           switch(key_msg)
//           {
//                case KEY_DOWN<<(KEY_HIT1*2) :
//                                             rtn+=5;
//                                             
//                                             insert_queue_bkg_persent(rtn);
//                                            
//                                             disp_int(get_matrix_data_p()->fg_color_str,WHITE ,rtn);
//                                             break;
//                case KEY_DOWN<<(KEY_HIT2*2) :
//                                             rtn -=5;
//                                             insert_queue_bkg_persent(rtn);
//                                             disp_int(get_matrix_data_p()->fg_color_str,WHITE ,rtn);
//                                             break;
//
//                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
//                                             disp_get_item_p()->m_item ++;
//                                             break;
//                case KEY_DOWN<<(KEY_SHT*2) :
//                                            disp_get_item_p()->s_item ++;
//                                             break;
//
//               default:
//                                             break;
//
//           }
//
//     }



     return rtn; 
}

/***********************************以下是单个锤头的显示函数******************************************************************/



uint8_t refresh_hit_blk_hook(void *p)
{

     uint32_t ch ;
     uint8_t str[8];

     ch = (uint32_t)p;

     sprintf((char *)str,"%4d",get_hit_p(ch)->blk_pro+get_hit_p(ch)->blk_pro_add);

     disp_str(get_matrix_data_p()->fg_color_str,BLUE,str);
     disp_or_char(get_matrix_data_p()->fg_color_str,0,RED,S_BLK_CHAR);
     change_timer(tm_odd_p,1000);


     return  ch ;
  
}

uint8_t refresh_hit_lock_hook(void* p)
{
     uint32_t ch ;
     uint8_t str[8];

     ch = (uint32_t)p;

     sprintf((char *)str,"%4d",get_hit_p(ch)->lock_pro+get_hit_p(ch)->lock_pro_add);

     disp_str(get_matrix_data_p()->fg_color_str,BLUE,str);
     disp_or_char(get_matrix_data_p()->fg_color_str,0,RED,HIT_CHAR);
     change_timer(tm_odd_p,1000);


     return  ch ;
}
uint8_t refresh_hit_big_hook(void *p)
{
     uint32_t ch ;
     uint8_t str[8];

     ch = (uint32_t)p;

     sprintf((char *)str,"%4d",get_hit_p(ch)->big_pro+get_hit_p(ch)->big_pro_add);

     disp_str(get_matrix_data_p()->fg_color_str,BLUE,str);
     disp_or_char(get_matrix_data_p()->fg_color_str,0,RED,BIG_CHAR);
     change_timer(tm_odd_p,1000);


     return  ch ;
}


uint8_t refresh_lkg_hook(void *p)
{
     uint32_t ch ;
     uint8_t str[8];

     ch = (uint32_t)p;

     sprintf((char *)str,"%4d",get_hit_p(ch)->lkg_pro+get_hit_p(ch)->lkg_pro_add);

     disp_str(get_matrix_data_p()->fg_color_str,BLUE,str);
     disp_or_char(get_matrix_data_p()->fg_color_str,0,RED,LKG_CHAR);
     change_timer(tm_odd_p,1000);


     return  ch ;
}


uint8_t refresh_brk_hook(void *p)
{
     uint32_t ch ;
     uint8_t str[8];

     ch = (uint32_t)p;

     sprintf((char *)str,"%4d",get_hit_p(ch)->line_break);

     disp_str(get_matrix_data_p()->fg_color_str,BLUE,str);
     disp_or_char(get_matrix_data_p()->fg_color_str,0,RED,BRK_CHAR);
     change_timer(tm_odd_p,1000);


     return  ch ;
}


uint8_t refresh_hit_volt_hook(void *p)
{
     uint32_t ch ;

     ch = (uint32_t)p;


     disp_float(get_matrix_data_p()->fg_color_str,YELLOW,get_hit_p(ch)->ad_value*2.5*5.7/4096);

     change_timer(tm_odd_p,200);


     return  ch ;
}

uint8_t  refresh_idle_volt_hook(void*p)
{

       uint32_t ch ;

     ch = (uint32_t)p;

     disp_int(get_matrix_data_p()->fg_color_str,YELLOW,get_hit_p(ch)->idle_volt);
     disp_or_char(get_matrix_data_p()->fg_color_str,0,YELLOW,'V');

     change_timer(tm_odd_p,1000);

     return ch;
}
uint8_t  refresh_idle_dev_hook(void * p)
{
    
     uint32_t ch ;

       ch = (uint32_t)p;
      disp_int(get_matrix_data_p()->fg_color_str,YELLOW,get_hit_p(ch)->idle_dev);
      disp_or_char(get_matrix_data_p()->fg_color_str,0,YELLOW,'D');

      change_timer(tm_odd_p,1000);

      return ch ;
}

uint8_t  refresh_nor_ratio_hook(void *p)
{
     
         
      uint32_t ch ;

      ch = (uint32_t)p;
      disp_int(get_matrix_data_p()->fg_color_str,LBLUE,get_hit_p(ch)->nor_ratio);
      disp_or_char(get_matrix_data_p()->fg_color_str,0,LBLUE,'N');
      change_timer(tm_odd_p,1000);

      return ch ;

}

uint8_t  refresh_high_ratio_hook(void *p)
{
     
         
      uint32_t ch ;

      ch = (uint32_t)p;
      disp_int(get_matrix_data_p()->fg_color_str,LBLUE,get_hit_p(ch)->high_ratio);
      disp_or_char(get_matrix_data_p()->fg_color_str,0,LBLUE,'H');
      change_timer(tm_odd_p,1000);

      return ch ;

}

uint8_t  refresh_low_ratio_hook(void *p)
{
     
         
      uint32_t ch ;

      ch = (uint32_t)p;
      disp_int(get_matrix_data_p()->fg_color_str,LBLUE,get_hit_p(ch)->low_ratio);
      disp_or_char(get_matrix_data_p()->fg_color_str,0,LBLUE,'L');
      change_timer(tm_odd_p,1000);

      return ch ;

}
/************************************************************************************************/
void hit_manage(uint8_t ch)
{

    uint16_t key_msg ;
    uint8_t str[8];
    uint8_t adj_var,mod;
    static uint8_t  last_ch=0;

     disp_get_item_p()->s_item =1;

     while(get_dy_p()->state == DY_BUSY)
     {
         disp_dynamic_data(); 
     }

     sprintf((char *)str,"HIT%d",ch);                       //动态显示HIT X 进入单个锤头设置的过门
     disp_str(get_dy_p()->data,PURPLE,str);
     get_dy_p()->type =  DY_DATA;

     if(ch != last_ch)
     {
        get_dy_p()->dir= DIR_R_L; 
        get_dy_p()->interval = 40;
       
     }
     else
     { 
         get_dy_p()->dir= DIR_D_U; 
         get_dy_p()->interval = 100;
     }
     last_ch = ch;     
     disp_dynamic_data();



      ican_ask_hit_set(ch);
      while(get_dy_p()->state == DY_BUSY);           //进入打壳时间子项的动态显示
      adj_var =get_hit_p(ch-1)->hit_time;
      mod = 0 ;                                       //此值表示修改状态 1为在修改状态

      sprintf((char *)str,"D\7%2d",adj_var);          //动态切换至具体的项目
      disp_str(get_dy_p()->data,GREEN,str);
      get_dy_p()->type =  DY_DATA;
      get_dy_p()->dir= DIR_D_U;
      get_dy_p()->interval = 50;
      disp_dynamic_data();

     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 1))  //显示和调整打壳时间
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                             if(mod == 0)
                                             {
                                                ican_add_hit_num(ch);
                                             }
                                             else
                                             {
                                                 adj_var ++;
                                                 if(adj_var >80)
                                                 {
                                                    adj_var = 80;
                                                 }
                                                 sprintf((char *)str,"D\7%2d",adj_var);
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                              if(mod == 0)
                                             {
                                                
                                             }
                                             else
                                             {
                                                 adj_var --;
                                                 if(adj_var <10)
                                                 {
                                                    adj_var =10;
                                                 }
                                                 sprintf((char *)str,"D\7%2d",adj_var);
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :
                                            if(mod == 0)
                                            {
                                                mod= 1;
                                                sprintf((char *)str,"D\7%2d",adj_var);
                                                disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                            }
                                            else
                                            {
                                               mod = 0;
                                               get_hit_p(ch-1)->hit_time=adj_var ;
                                               sprintf((char *)str,"D\7%2d",adj_var);
                                               disp_str(get_matrix_data_p()->fg_color_str,GREEN,str);
                                               ican_set_hit_mode(ch);

                                            }

                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                            if(mod == 0)
                                            {
                                                disp_get_item_p()->s_item ++;
                                            }
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             if(mod == 0)
                                             {
                                                disp_get_item_p()->m_item ++;
                                             }
                                             break;
               default:
                                             break;

           }

     }



     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 2))       //进入打壳间隔设置子项的动画显示
     {
          while(get_dy_p()->state == DY_BUSY);
          adj_var =get_hit_p(ch-1)->hit_int_time;
          mod = 0 ;                                    //此值表示修稿状态 1为在修改状态
          sprintf((char *)str,"U\x1c%2d",adj_var);
          disp_str(get_dy_p()->data,GREEN,str);
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();
     }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 2))  //显示和调整打壳间隔
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                             if(mod == 0)
                                             {
                                                ican_add_hit_num(ch);
                                             }
                                             else
                                             {
                                                 adj_var ++;
                                                 if(adj_var >80)
                                                 {
                                                    adj_var = 80;
                                                 }
                                                 sprintf((char *)str,"U\x1c%2d",adj_var);
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                              if(mod == 0)
                                             {
                                                
                                             }
                                             else
                                             {
                                                 adj_var --;
                                                 if(adj_var <10)
                                                 {
                                                    adj_var =10;
                                                 }
                                                 sprintf((char *)str,"U\x1c%2d",adj_var);
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :
                                            if(mod == 0)
                                            {
                                                mod= 1;
                                                sprintf((char *)str,"U\x1c%2d",adj_var);
                                                disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                            }
                                            else
                                            {
                                               mod = 0;
                                               get_hit_p(ch-1)->hit_int_time=adj_var ;
                                               sprintf((char *)str,"U\x1c%2d",adj_var);
                                               disp_str(get_matrix_data_p()->fg_color_str,GREEN,str);
                                               ican_set_hit_mode(ch);

                                            }

                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             if(mod  == 0)
                                             {
                                                 disp_get_item_p()->s_item ++;
                                             }
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             if(mod == 0)
                                             {
                                                 disp_get_item_p()->m_item ++;
                                             }
                                             break;
               default:
                                             break;

           }

     }

     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 3))   //进入打壳保持时间设置的动画
     {
          while(get_dy_p()->state == DY_BUSY);
          adj_var =get_hit_p(ch-1)->hold_time;
          mod = 0 ;                                    //此值表示修稿状态 1为在修改状态
          sprintf((char *)str,"H %2d",adj_var);
          disp_str(get_dy_p()->data,GREEN,str);
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();
      }

     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 3))  //显示和调整打壳保持时间
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                             if(mod == 0)
                                             {
                                                ican_add_hit_num(ch);
                                             }
                                             else
                                             {
                                                 adj_var ++;
                                                 if(adj_var >80)
                                                 {
                                                    adj_var = 80;
                                                 }
                                                 sprintf((char *)str,"H %2d",adj_var);
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                              if(mod == 0)
                                             {
                                                
                                             }
                                             else
                                             {
                                                 adj_var --;
                                                 if(adj_var <10)
                                                 {
                                                    adj_var =10;
                                                 }
                                                 sprintf((char *)str,"H %2d",adj_var);
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :
                                            if(mod == 0)
                                            {
                                                mod= 1;
                                                sprintf((char *)str,"H %2d",adj_var);
                                                disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                            }
                                            else
                                            {
                                               mod = 0;
                                               get_hit_p(ch-1)->hold_time=adj_var ;
                                               sprintf((char *)str,"H %2d",adj_var);
                                               disp_str(get_matrix_data_p()->fg_color_str,GREEN,str);
                                               ican_set_hit_mode(ch);

                                            }

                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             if(mod == 0)
                                             {
                                                 disp_get_item_p()->s_item ++;
                                             }
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             if(mod == 0)
                                             {
                                                 disp_get_item_p()->m_item ++;
                                             }
                                             break;
               default:
                                             break;

           }

     }




  /****************************************显示堵值***********************************************************/
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 4))
     {
          while(get_dy_p()->state == DY_BUSY);
          adj_var =get_hit_p(ch-1)->blk_pro+get_hit_p(ch-1)->blk_pro_add;

          sprintf((char *)str,"%4d",adj_var);

          disp_str(get_dy_p()->data,BLUE,str);
          disp_or_char(get_dy_p()->data,0,RED,S_BLK_CHAR);
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_hit_blk_hook);           //建立定时器 定时刷新前景色  
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 4))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;






    /*********************************显示卡锤头值***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 5))
     {
          while(get_dy_p()->state == DY_BUSY);
          adj_var =get_hit_p(ch-1)->lock_pro+get_hit_p(ch-1)->lock_pro_add;

          sprintf((char *)str,"%4d",adj_var);

          disp_str(get_dy_p()->data,BLUE,str);
          disp_or_char(get_dy_p()->data,0,RED,HIT_CHAR);
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_hit_lock_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 5))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;




         /*********************************显示大葫芦头***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 6))
     {
          while(get_dy_p()->state == DY_BUSY);
          adj_var =get_hit_p(ch-1)->big_pro+get_hit_p(ch-1)->big_pro_add;

          sprintf((char *)str,"%4d",adj_var);

          disp_str(get_dy_p()->data,BLUE,str);
          disp_or_char(get_dy_p()->data,0,RED,BIG_CHAR);
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_hit_big_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 6))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;



     /*********************************显示漏电***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 7))
     {
          while(get_dy_p()->state == DY_BUSY);
          adj_var =get_hit_p(ch-1)->lkg_pro+get_hit_p(ch-1)->lkg_pro_add;

          sprintf((char *)str,"%4d",adj_var);

          disp_str(get_dy_p()->data,BLUE,str);
          disp_or_char(get_dy_p()->data,0,RED,LKG_CHAR);
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_lkg_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 7))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;




    /*********************************显示断线***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 8))
     {
          while(get_dy_p()->state == DY_BUSY);
          adj_var =get_hit_p(ch-1)->line_break;

          sprintf((char *)str,"%4d",adj_var);

          disp_str(get_dy_p()->data,BLUE,str);
          disp_or_char(get_dy_p()->data,0,RED,BRK_CHAR);
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_brk_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 8))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;




 /*********************************显示即时电压***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 9))
     {
          while(get_dy_p()->state == DY_BUSY);
         

          disp_float(get_dy_p()->data,YELLOW,get_hit_p(ch-1)->ad_value*2.5*5.7/4096);
      
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_hit_volt_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 9))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;



     /*********************************显示闲时电压***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 10))
     {
          while(get_dy_p()->state == DY_BUSY);
         

          disp_int(get_dy_p()->data,YELLOW,get_hit_p(ch-1)->idle_volt);
          disp_or_char(get_dy_p()->data,0,YELLOW,'V');
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_idle_volt_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 10))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;



          /*********************************显示闲时电压f方差***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 11))
     {
          while(get_dy_p()->state == DY_BUSY);
         

          disp_int(get_dy_p()->data,YELLOW,get_hit_p(ch-1)->idle_dev);
          disp_or_char(get_dy_p()->data,0,YELLOW,'D');
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_idle_dev_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 11))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                            ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;



    /*********************************正常曲线的比例值***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 12))
     {
          while(get_dy_p()->state == DY_BUSY);
         

          disp_int(get_dy_p()->data,LBLUE,get_hit_p(ch-1)->nor_ratio);
          disp_or_char(get_dy_p()->data,0,LBLUE,'N');
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_nor_ratio_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 12))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;



         /*********************************G高电压曲线的比例值***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 13))
     {
          while(get_dy_p()->state == DY_BUSY);
         

          disp_int(get_dy_p()->data,LBLUE,get_hit_p(ch-1)->high_ratio);
          disp_or_char(get_dy_p()->data,0,LBLUE,'H');
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_high_ratio_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 13))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;



              /*********************************低电压曲线的比例值***************************************************************/ 
     if((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 14))
     {
          while(get_dy_p()->state == DY_BUSY);
         

          disp_int(get_dy_p()->data,LBLUE,get_hit_p(ch-1)->low_ratio);
          disp_or_char(get_dy_p()->data,0,LBLUE,'L');
          get_dy_p()->type =  DY_DATA;
          get_dy_p()->dir= DIR_D_U;
          get_dy_p()->interval = 50;
          disp_dynamic_data();

          tm_odd_p  = creat_timer((void *)(ch-1),   1000 , refresh_low_ratio_hook);           //
      }
     while((disp_get_item_p()->m_item == ch+1)&&(disp_get_item_p()->s_item == 14))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :
                                           
                                                ican_add_hit_num(ch);
                                           
                               
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :


                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                             ican_reversal_hit_hold(ch);
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item ++;
                                             break;
               default:
                                             break;

           }

     }
     destory_timer(tm_odd_p);
     tm_odd_p = 0;

}

/*******************************************版本管理函数**************************************************************************/


uint8_t refresh_time_hook(void * p)
{
   struct tm *tm_p;

   uint8_t str[8];

   tm_p = p;

   RTC_GetCalendar(tm_p) ; 


   sprintf((char *)str,"%2d%2d",tm_p->tm_hour,tm_p->tm_min);          //动态切换至具体的项目
   disp_str(get_matrix_data_p()->fg_color_str,GREEN,str);

   if(tm_p->tm_sec %2 == 0)
   {
     disp_point(get_matrix_data_p()->fg_color_str,6,15,GREEN);
   }

   change_timer(tm_odd_p,1000);

   return 0;
}


uint8_t refresh_data_hook(void * p)
{
   struct tm *tm_p;

   uint8_t str[8];

   tm_p = p;

   RTC_GetCalendar(tm_p) ; 


   sprintf((char *)str,"%2d%2d",tm_p->tm_mon,tm_p->tm_mday);          //动态切换至具体的项目
   disp_str(get_matrix_data_p()->fg_color_str,GREEN,str);

   if(tm_p->tm_sec %2 == 0)
   {
     disp_point(get_matrix_data_p()->fg_color_str,6,15,GREEN);
   }

   change_timer(tm_odd_p,1000);

   return 0;
}

void other_manage(void)
{
   

    uint16_t key_msg ;
    uint8_t str[20];
    struct tm time;
    uint16_t adj_var1,adj_var2,mod;


     disp_get_item_p()->s_item =1;

     while(get_dy_p()->state == DY_BUSY)
     {
         disp_dynamic_data(); 
     }

     sprintf((char *)str,"VER3");                       //动态显示ver3  表示是第3个版本的程序
     disp_str(get_dy_p()->data,PURPLE,str);
     get_dy_p()->type =  DY_DATA;
     get_dy_p()->dir= DIR_R_L; 
     get_dy_p()->interval = 40;   
     disp_dynamic_data();




      while(get_dy_p()->state == DY_BUSY);           //动态显示外CAN地址
      adj_var1 =get_addr();
      mod = 0 ;                                       //此值表示修改状态 1为在修改状态

      sprintf((char *)str,"%4d",adj_var1);          //动态切换至具体的项目
      disp_str(get_dy_p()->data,GREEN,str);
      disp_or_char(get_dy_p()->data,0,GREEN,CANID_CAHR);
      get_dy_p()->type =  DY_DATA;
      get_dy_p()->dir= DIR_D_U;
      get_dy_p()->interval = 50;
      disp_dynamic_data();

     while((disp_get_item_p()->m_item == 8)&&(disp_get_item_p()->s_item == 1))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :                                           
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :
                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                            
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item =1;
                                             break;
               default:
                                             break;

           }

     }



/***************************************时间设置**********************************************************************/

     while(get_dy_p()->state == DY_BUSY);           //等待运动结束
     if((disp_get_item_p()->m_item == 8)&&(disp_get_item_p()->s_item == 2))
     {
        while(get_dy_p()->state == DY_BUSY);           //进
   
         RTC_GetCalendar(&time) ; 
         adj_var1 =  time.tm_hour ;
         adj_var2 =  time.tm_min ;
         mod = 0 ;                                       //此值表示修改状态 1为在修改状态

      sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
      disp_str(get_dy_p()->data,GREEN,str);
      
      get_dy_p()->type =  DY_DATA;
      get_dy_p()->dir= DIR_D_U;
      get_dy_p()->interval = 50;
      disp_dynamic_data();

     // tm_odd_p  = creat_timer((void *)&time,   1000 , refresh_time_hook);           //
     }
      while((disp_get_item_p()->m_item == 8)&&(disp_get_item_p()->s_item == 2))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :   
                                             if(mod!=0)
                                             {
                                                 adj_var1 ++;
                                                 if(adj_var1>=24)
                                                 {
                                                   adj_var1 = 0 ;
                                                 }
                                                 
                                                 sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }                                        
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :

                                             if(mod!=0)
                                             {
                                                 adj_var2 ++;
                                                 if(adj_var2>=60)
                                                 {
                                                   adj_var2 = 0 ;
                                                 }
                                                 
                                                 sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             } 
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :
                                             if(mod == 0)
                                             {
                                                 mod = 1;
                                                
                                                 RTC_GetCalendar(&time) ; 
                                                 adj_var1 =  time.tm_hour ;
                                                 adj_var2 =  time.tm_min ;
                                                 sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);

                                                 destory_timer(tm_odd_p);
                                                 tm_odd_p =0;
                                             }else
                                             {
                                                 mod = 0 ;
                                                 time.tm_hour = adj_var1;
                                                 time.tm_min  = adj_var2;
                                                 Time_SetCalendar(&time);
                                                 tm_odd_p  = creat_timer((void *)&time,   10 , refresh_time_hook); 

                                             }
                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                            
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item =1;
                                             break;
               default:
                                             break;

           }

     }

      destory_timer(tm_odd_p);
       tm_odd_p =0;



    /***************************************日期设置**********************************************************************/
       while(get_dy_p()->state == DY_BUSY);           //等待运动结束
     if((disp_get_item_p()->m_item == 8)&&(disp_get_item_p()->s_item == 3))
     {
        while(get_dy_p()->state == DY_BUSY);           //进
   
         RTC_GetCalendar(&time) ; 
         adj_var1 =  time.tm_mon ;
         adj_var2 =  time.tm_mday ;
         mod = 0 ;                                       //此值表示修改状态 1为在修改状态

      sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
      disp_str(get_dy_p()->data,GREEN,str);
      
      get_dy_p()->type =  DY_DATA;
      get_dy_p()->dir= DIR_D_U;
      get_dy_p()->interval = 50;
      disp_dynamic_data();

      tm_odd_p  = creat_timer((void *)&time,   1000 , refresh_data_hook);           //
     }
      while((disp_get_item_p()->m_item == 8)&&(disp_get_item_p()->s_item == 3))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :   
                                             if(mod!=0)
                                             {
                                                 adj_var1 ++;
                                                 if(adj_var1>=12)
                                                 {
                                                   adj_var1 = 0 ;
                                                 }
                                                 
                                                 sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }                                        
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :

                                             if(mod!=0)
                                             {
                                                 adj_var2 ++;
                                                 if(adj_var2>=32)
                                                 {
                                                   adj_var2 = 0 ;
                                                 }
                                                 
                                                 sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             } 
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :
                                             if(mod == 0)
                                             {
                                                 mod = 1;
                                                
                                                 RTC_GetCalendar(&time) ; 
                                                 adj_var1 =  time.tm_mon ;
                                                 adj_var2 =  time.tm_mday ;
                                                 sprintf((char *)str,"%2d%2d",adj_var1,adj_var2);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);

                                                 destory_timer(tm_odd_p);
                                                 tm_odd_p =0;
                                             }else
                                             {
                                                 mod = 0 ;
                                                 time.tm_mon = adj_var1;
                                                 time.tm_mday  = adj_var2;
                                                 Time_SetCalendar(&time);
                                                 tm_odd_p  = creat_timer((void *)&time,   10 , refresh_data_hook); 

                                             }
                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                            
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item =1;
                                             break;
               default:
                                             break;

           }

     }

      destory_timer(tm_odd_p);
      tm_odd_p =0;



 /***************************************年设置**********************************************************************/
       while(get_dy_p()->state == DY_BUSY);           //等待运动结束
     if((disp_get_item_p()->m_item == 8)&&(disp_get_item_p()->s_item == 4))
     {
        while(get_dy_p()->state == DY_BUSY);           //进
   
         RTC_GetCalendar(&time) ; 
         adj_var1 = time.tm_year;
   
         mod = 0 ;                                       //此值表示修改状态 1为在修改状态

      sprintf((char *)str,"%4d",adj_var1);          //动态切换至具体的项目
      disp_str(get_dy_p()->data,GREEN,str);
      
      get_dy_p()->type =  DY_DATA;
      get_dy_p()->dir= DIR_D_U;
      get_dy_p()->interval = 50;
      disp_dynamic_data();

    
     }
      while((disp_get_item_p()->m_item == 8)&&(disp_get_item_p()->s_item == 4))  //
     {
         key_msg = get_key_message(get_key_p(0));

           switch(key_msg)
           {
                case KEY_DOWN<<(KEY_HIT1*2) :   
                                             if(mod!=0)
                                             {
                                                 adj_var1 ++;                                                 
                                                 sprintf((char *)str,"%4d",adj_var1);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             }                                        
                                             break;

                case KEY_DOWN<<(KEY_HIT2*2) :

                                             if(mod!=0)
                                             {
                                                 adj_var1 --;                                                 
                                                 sprintf((char *)str,"%4d",adj_var1);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);
                                             } 
                                             break;
                case KEY_DOWN<<(KEY_SET*2) :
                                             if(mod == 0)
                                             {
                                                 mod = 1;
                                                

                                                 sprintf((char *)str,"%4d",adj_var1);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,RED,str);

                                             }else
                                             {
                                                 mod = 0 ;
                                                 time.tm_year = adj_var1;
                                                
                                                 Time_SetCalendar(&time);

                                                 sprintf((char *)str,"%4d",adj_var1);          //动态切换至具体的项目
                                                 disp_str(get_matrix_data_p()->fg_color_str,GREEN,str);

                                             }
                                             break;
                case KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->s_item ++;
                                             break;
                case KEY_HOLD<<(KEY_HIT1*2) :
                                            
                                             break;
                case KEY_HOLD<<(KEY_SET*2)|KEY_DOWN<<(KEY_SHT*2) :
                                             disp_get_item_p()->m_item =1;
                                             break;
               default:
                                             break;

           }

     }



}













