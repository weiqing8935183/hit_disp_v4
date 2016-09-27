#include "SysTick.h"

TIMER_STRUCT_INFO  timer_str[TIMER_NUM];
TIMER_LIST_INFO    free_list,work_list;
/*
 * 函数名：SysTick_Init
 * 描述  ：启动系统滴答定时器 SysTick
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用 
 */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// 
	if (SysTick_Config(SystemCoreClock / 1000))	//  100us中断一次
	{ 
		/* Capture error */ 
		while (1);
	}
		// 关闭滴答定时器  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;


    //使能滴答定时器
    //SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}


//初始化定时器的链表
void init_timer_list(void)
{
     uint8_t i;

     for(i=0;i<TIMER_NUM;i++)
     {
        timer_str[i].timer_para   = 0;
        timer_str[i].dec_num     = 0;
        timer_str[i].over_fun    = 0;
        timer_str[i].next_p      = timer_str+i+1;
     }

     timer_str[TIMER_NUM-1].next_p=0;


      free_list.head_p   =   timer_str;
      free_list.list_num =   TIMER_NUM ;

      work_list.head_p = 0;
      work_list.list_num = 0;
}



//创建定时器
//返回值为创建的定时器指针
//若返回0 认为创建失败

//flag 为 (*fun)(void * ) 的入口参数
//interval 为定时器的中断次数  即间隔次数
//(*fun)(void * ) 为超时后调用的指针

TIMER_STRUCT_INFO * creat_timer(void * flag,uint32_t  interval ,uint8_t (*fun)(void * ))
{
   TIMER_STRUCT_INFO * rtn=0;

   TIMER_STRUCT_INFO * tm_p;

   if( free_list.list_num != 0)
   {
       //从空闲列表中删除一个结构体
       tm_p =  free_list.head_p ;
       free_list.head_p = tm_p->next_p;
       free_list.list_num -- ;


       //定时器的结构体赋值
       tm_p->timer_para  = flag ;
       tm_p->dec_num     = interval;
       tm_p->set_num     = interval;
       tm_p->over_fun    = fun;


       if(work_list.list_num !=0)    //如果已经有定时器模块
       {
           tm_p->next_p = work_list.head_p;
           work_list.head_p   =  tm_p;
             
           work_list.list_num    ++;


       }
       else                            //如果没有定时器模块在执行
       {
          
            work_list.head_p   =  tm_p;
            tm_p->next_p       =  0;
            work_list.list_num    ++;
                 //使能滴答定时器
            SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
       }

       rtn =  work_list.head_p;
   }
   else
   {
        rtn = 0 ;//零表示创建定时器失败
   }


   return rtn;
}

//刷新一个定时器
uint8_t  refresh_timer(TIMER_STRUCT_INFO *tm_p)
{
    uint8_t rtn=0;
    
    



    if(tm_p != 0)
    {
        tm_p->dec_num = tm_p->set_num;
    }
    else
    {
        rtn =1 ;
    }


    return rtn;
}

//刷新一个定时器
//改变一个定时器的参数 ，并且s刷新他
uint8_t  change_timer(TIMER_STRUCT_INFO *tm_p,uint32_t interval)
{
    uint8_t rtn=0;
    
    



    if(tm_p != 0)
    {
        tm_p->set_num =  interval;
        tm_p->dec_num =  interval;
    }
    else
    {
        rtn =1 ;
    }


    return rtn;
}
//销毁一个定时器
uint8_t  destory_timer(TIMER_STRUCT_INFO *tm_p)
{

    uint8_t rtn=0;
    TIMER_STRUCT_INFO *pre_p=0;
    TIMER_STRUCT_INFO *now_p= work_list.head_p ;

    if((tm_p!=0)&&(now_p!=0))
    {
        for(;(now_p != tm_p)&&(now_p != 0);pre_p=now_p,now_p= now_p->next_p);

        if((pre_p == 0)&&(now_p == tm_p)) //说明是第一个
        {
          //删除该节点
          work_list.head_p = tm_p->next_p;
          work_list.list_num    --;

          if(work_list.list_num == 0)
          {
             // 关闭滴答定时器  
	         SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk; 
          }

          tm_p->next_p = free_list.head_p;
          free_list.head_p = tm_p;
          free_list.list_num ++;

         }
         else if((pre_p != 0)&&(now_p  == tm_p))
        {
           //删除中间的节点
           pre_p->next_p = tm_p->next_p;
           work_list.list_num    --;

          tm_p->next_p = free_list.head_p;
          free_list.head_p = tm_p;
          free_list.list_num ++;
        }
  
    }
    return rtn;

}


//定时器定时中断
uint8_t timer_prd_irq(void)
{
    

    TIMER_STRUCT_INFO *now_p ,*tem_p;

    for(now_p = work_list.head_p;  now_p!=0; )
    {
       now_p->dec_num --;
       tem_p = now_p->next_p ;
       if(now_p->dec_num == 0)
       {
          

           now_p->over_fun(now_p->timer_para);
           if(now_p->dec_num == 0)
           {
             destory_timer(now_p);
           }

         
       }
       now_p = tem_p;
    }


  return   work_list.list_num;
}


