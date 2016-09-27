#ifndef __INIT_SPI_H
#define __INIT_SPI_H


#include "stm32f10x.h"

uint8_t init_spi(void);
void spi2_nvic_config(void);


void spi2_dma_nvic_config(void);
uint8_t spi2_dma_init(void);
uint8_t  start_dma_send(uint8_t *str_p ,uint16_t num);






#endif



