/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "systick.h"
#include "in_can.h"
#include "init_mcp2515.h"
#include "out_can.h"
#include "dm163.h"
#include "disp.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    timer_prd_irq();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

void TIM2_IRQHandler (void)
{
     static int tim_acc=0;
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)  //����Ƕ�ʱ�������ж�
   {
       tim_acc ++;
       ican_ask_sequence_prd();
       disp_m_item();
       if(tim_acc>= 50)
       {
         out_can_prd();
         tim_acc = 0;
       }
	   TIM_ClearFlag (TIM2,TIM_FLAG_Update);//�����ʱ�����ж�
   
    }
}


/****************************************************
�����жϺ��� 


*****************************************************/
void EXTI4_IRQHandler(void)
{
  uint8_t i;
  CanRxMsg Out_RxMessage;

  if(EXTI_GetITStatus(EXTI_Line4) != RESET) //ȷ���Ƿ������EXTI Line�ж�
  {
          i = MCP2515_IT_Get();
          if(i&0x03)              //�����ж�
          {
            MCP2515_CAN_Receive(&Out_RxMessage);
            out_can_rec_analyse(&Out_RxMessage);
         
          }
          MCP2515_IT_Clear(i);



    EXTI_ClearITPendingBit(EXTI_Line4);     //����жϱ�־λ
  }  
}

/*******************************************
********************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)  //CAN�ж�
{

 CanRxMsg RxMessage;

  if(CAN_GetITStatus  (CAN1,CAN_IT_FMP0 )!=RESET)
  {
     CAN_GetITStatus (CAN1,CAN_IT_FMP0);
     CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
     ican_rec_analyse( &RxMessage);
  }

}

void USB_HP_CAN1_TX_IRQHandler(void)
{
  if(CAN_GetITStatus  (CAN1,CAN_IT_TME )!=RESET)
  {
       CAN_ClearITPendingBit  (CAN1, CAN_IT_TME  ); 
       can_queue_send_irq(get_canbus_queue_p());
       
  }
}


void DMA1_Channel5_IRQHandler(void)
{
    
     if(end_matrix_row_send_irp(get_matrix_crl_p())!=0)
     {
        DMA_Cmd(DMA1_Channel5, DISABLE);
     }
     DMA_ClearITPendingBit  ( DMA1_FLAG_TC5  );  

}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


