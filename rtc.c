#include "rtc.h"



void RTC_Config(void)
{

  
  	 RCC_APB1PeriphClockCmd (RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE)	;//������Դ���ƺͺ󱸼Ĵ�������
	 PWR_BackupAccessCmd(ENABLE);											  //��Դ���ֽ���Ժ󱸼Ĵ����ı���
	 RCC_LSEConfig (RCC_LSE_ON);											  //����LSEʱ��
	 while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)!=SET )                          //�ȴ�LSEʱ���ȶ�
	 RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);								  //ѡ��LSEΪRTCʱ��
	 RCC_RTCCLKCmd(ENABLE);												      //����RTC����ʱ��

	/* Wait for RTC registers synchronization */
	 RTC_WaitForSynchro();													  //�ȴ�ͬ�����

   	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, DISABLE);										  //off RTC�����ж�

	
	 	
	 RTC_WaitForLastTask();	                                                  //�ȴ���һ�ε��������
	 RTC_SetPrescaler (0X7FFF);												  //����Ԥ��Ƶ��Ϊ32767
	 RTC_WaitForLastTask();													  //�ȴ��������

	 //BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);								  //��RTC������ɺ󣬽����ݼĴ���1��Ϊ0XA5A5���´�����ʱ�ɼ�⣬ʵʱʱ���Ƿ�ʧ

	  
}
/*******************************************************/
/*            ʵʱʱ���ж����ȼ�����                       */
/*            ��Ҫ�����жϷ���������������               */
/*            ���û��ʵʱʱ�ӵ��жϿ��Բ�����             */
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
/***********************���ʵʱʱ�ӵ�32λ��������ֵ******************************/
uint32_t RTC_GetUTime(void)
{
    return (RTC_GetCounter());
}
/***********************���ʵʱʱ�ӵ�ֵ ����ֵ��tm�ṹ���У�*****************************/
void  RTC_GetCalendar(struct tm *t)
{
    
    uint32_t  temp;
    temp =  RTC_GetCounter();
	ConvertUTimeToCalendar(t,temp);

}
