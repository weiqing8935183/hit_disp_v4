
#include "init_tim.h"

//定时器2初始化程序
//Time_Inteval 定时中断的时间间隔 单位ms

void Tim2Config(uint16_t  Time_Inteval)
{

  TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct ;



  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);


        TIM_DeInit(TIM2);

        if(Time_Inteval<=50)
        {
		TIM_TimeBaseInitStruct.TIM_Period=Time_Inteval*1000; // Time_Inteval单位为ms  所以要乘1000   暂时改为100 表示100us为一单位
        TIM_TimeBaseInitStruct.TIM_Prescaler=(72-1);         //将其72分频 即TIM 的内部时钟为1us一计数
        TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
        TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
        
        }
        else
        {
        TIM_TimeBaseInitStruct.TIM_Period=Time_Inteval*10;  // Time_Inteval单位为ms  定时器的时间单位是100us 所以要乘10
        TIM_TimeBaseInitStruct.TIM_Prescaler=(7200-1);      //将其7200分频 即TIM 的内部时钟为100us一计数
        TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
        TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
        
        }

        TIM_TimeBaseInit( TIM2,&TIM_TimeBaseInitStruct  ) ;


	  //TIM_OC3PreloadConfig (TIM2,TIM_OCPreload_Enable);
      //TIM_OC4PreloadConfig (TIM2,TIM_OCPreload_Enable);

     //TIM_ClearFlag(TIM2,TIM_FLAG_Update);
     //TIM_ClearFlag(TIM2,TIM_FLAG_CC3);
      TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
     
      //TIM_Cmd(TIM2,ENABLE);
}
/*
 * 函数名：TIM2_NVIC_Configuration
 * 描述  ：TIM2中断优先级配置
 * 输入  ：无
 * 输出  ：无	
 */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

