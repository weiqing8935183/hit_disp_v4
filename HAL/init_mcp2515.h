#ifndef  __INIT_MCP2515_H
#define  __INIT_MCP2515_H

#include "stm32f10x.h"

#define Dummy_Byte                0xFF
#define MCP2515_CMD_RESET                0xC0
#define MCP2515_CMD_READ_STATUE          0XA0


#define MCP2515_IT_RX0F                0x01
#define MCP2515_IT_RX1F                0x02
#define MCP2515_IT_TX0E                0x04
#define MCP2515_IT_TX1E                0x08
#define MCP2515_IT_TX2E                0x10
#define MCP2515_IT_ERRF                0x20
#define MCP2515_IT_WAKF                0x40
#define MCP2515_IT_MERF                0x80


#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)

void EXTI_PC4_Config(void);
void EXTI_PC4_NVIC_Config(void);

void  out_can_config(void);
void SPIPort_Init(void);


uint8_t MCP2515_RESET(void);
uint8_t MCP2515_READ_STATUE(void);
void    MCP2515_Init(void);
void    MCP2515_ENTERWORK(void);
void    MCP2515_ENTERCONFIG(void);
void    MCP2515_Filter_Set(uint8_t chanel,uint8_t IDE,uint32_t ID);
void    MCP2515_Mask_Set(uint8_t chanel,uint8_t IDE,uint32_t ID) ;
void    MCP2515_IT_Clear(uint8_t i);
void    MCP2515_IT_Enable(uint8_t i);
uint8_t MCP2515_IT_Get(void);

void    MCP2515_CAN_Receive(CanRxMsg *RxMessage);
void    MPC2515_CAN_SEND(CanTxMsg* TxMessage);   
#endif
