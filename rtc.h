#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"

void RTC_NVICConfig(void);
void RTC_Config(void);

#include "time.h"

void  RTC_GetCalendar(struct tm *t);

uint32_t RTC_GetUTime(void);

void Time_SetCalendar(struct tm * t);

#endif
