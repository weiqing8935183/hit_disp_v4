
#include "init_tim.h"

//��ʱ��2��ʼ������
//Time_Inteval ��ʱ�жϵ�ʱ���� ��λms

void Tim2Config(uint16_t  Time_Inteval)
{

  TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStruct ;



  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);


        TIM_DeInit(TIM2);

        if(Time_Inteval<=50)
        {
		TIM_TimeBaseInitStruct.TIM_Period=Time_Inteval*1000; // Time_Inteval��λΪms  ����Ҫ��1000   ��ʱ��Ϊ100 ��ʾ100usΪһ��λ
        TIM_TimeBaseInitStruct.TIM_Prescaler=(72-1);         //����72��Ƶ ��TIM ���ڲ�ʱ��Ϊ1usһ����
        TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
        TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
        
        }
        else
        {
        TIM_TimeBaseInitStruct.TIM_Period=Time_Inteval*10;  // Time_Inteval��λΪms  ��ʱ����ʱ�䵥λ��100us ����Ҫ��10
        TIM_TimeBaseInitStruct.TIM_Prescaler=(7200-1);      //����7200��Ƶ ��TIM ���ڲ�ʱ��Ϊ100usһ����
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
 * ��������TIM2_NVIC_Configuration
 * ����  ��TIM2�ж����ȼ�����
 * ����  ����
 * ���  ����	
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

