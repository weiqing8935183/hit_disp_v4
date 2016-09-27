#ifndef   __INIT_UART_H
#define   __INIT_UART_H


#include "stm32f10x.h"


void init_uart2_modbus(uint32_t baudrate) ; 
void USART2_Config(uint32_t baudrate);
void USART2NVICConfig(void);




void init_uart3_modbus(uint32_t baudrate);
void USART3_Config(uint32_t baudrate);
void USART3NVICConfig(void);

#endif


