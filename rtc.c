#include "rtc.h"



void RTC_Config(void)
{

  
  	 RCC_APB1PeriphClockCmd (RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE)	;//开启电源控制和后备寄存器部分
	 PWR_BackupAccessCmd(ENABLE);											  //电源部分解除对后备寄存器的保护
	 RCC_LSEConfig (RCC_LSE_ON);											  //开启LSE时钟
	 while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)!=SET )                          //等待LSE时钟稳定
	 RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);								  //选择LSE为RTC时钟
	 RCC_RTCCLKCmd(ENABLE);												      //开启RTC部分时钟

	/* Wait for RTC registers synchronization */
	 RTC_WaitForSynchro();													  //等待同步完成

   	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, DISABLE);										  //off RTC的秒中断

	
	 	
	 RTC_WaitForLastTask();	                                                  //等待上一次的任务完成
	 RTC_SetPrescaler (0X7FFF);												  //配置预分频器为32767
	 RTC_WaitForLastTask();													  //等待配置完成

	 //BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);								  //对RTC操作完成后，将备份寄存区1置为0XA5A5，下次启动时可检测，实时时钟是否丢失

	  
}
/*******************************************************/
/*            实时时钟中断优先级设置                       */
/*            需要根据中断分组的情况进行设置               */
/*            如果没有实时时钟的中断可以不调用             */
void RTC_NVICConfig(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
	

	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



void  ConvertUTimeToCalendar(struct tm *p,uint32_t UTime)
{
   	struct tm *pt;

    pt=localtime(&UTime);
    pt->tm_year += 1900;

    p->tm_year=pt->tm_year;
    p->tm_mon=pt->tm_mon;
    p->tm_mday=pt->tm_mday ;
    p->tm_hour=pt->tm_hour;
    p->tm_min=pt->tm_min;
    p->tm_sec=pt->tm_sec;
    p->tm_wday=pt->tm_wday;
    p->tm_yday=pt->tm_yday;
    p->tm_isdst=pt->tm_isdst;

}

uint32_t ConvertCalendarToUTime(struct tm * t)
{
   t->tm_year -=1900;
   return mktime(t);
}

void Time_SetUTime(uint32_t t)
{
  RTC_WaitForLastTask();
  RTC_SetCounter(t);
  RTC_WaitForLastTask();
}

void Time_SetCalendar(struct tm * t)
{	
// 	  Time_Now.tm_year=2014;  //
//	  Time_Now.tm_mon=8;	  //0-11
//	  Time_Now.tm_mday=18;	  //1-31
//	  Time_Now.tm_wday=3;	  //0-6
//	  Time_Now.tm_hour=8;	  //0-23
//	  Time_Now.tm_min=17;	  //0-59
//	  Time_Now.tm_sec=0;	  //0-60
//	  Time_SetCalendar(Time_Now);
   	  Time_SetUTime(ConvertCalendarToUTime(t));
}
/***********************获得实时时钟的32位计数器的值******************************/
uint32_t RTC_GetUTime(void)
{
    return (RTC_GetCounter());
}
/***********************获得实时时钟的值 ，其值在tm结构体中，*****************************/
void  RTC_GetCalendar(struct tm *t)
{
    
    uint32_t  temp;
    temp =  RTC_GetCounter();
	ConvertUTimeToCalendar(t,temp);

}
