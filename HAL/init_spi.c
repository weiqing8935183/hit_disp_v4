#include "init_spi.h"


 DMA_InitTypeDef    DMA_InitStructure;


uint8_t init_spi(void)
{
    uint8_t  rtn;

  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /*!< SPI_FLASH_SPI Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
// RST_BIT    //PB10
// LAT_BIT    //PB11
// SLB_BIT    //PB12
// EN_BIT     //PB14
// SCK_BIT    //PB13
// MOSI_BIT   //PB15


  /*!< PB10 IS RESET PIN */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  

    /*!< PB11 IS LAT PIN*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!<PB12 IS SLB PIN*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< PB13 Configure SPI_FLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*!<PB14 IS SLB PIN*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  /*!PB15 IS MOSI*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);



  /* SPI1 configuration */
  // W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
  // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
 

   //�������ж�����ν
  //SPI_I2S_ITConfig  ( SPI2, SPI_I2S_IT_TXE,   DISABLE ) ; 
  //ʹ��dma  ��dma�ж�
  SPI_I2S_DMACmd (SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
 
  /* Enable SPI2  */
  SPI_Cmd(SPI2, ENABLE);


  return rtn;

}


/*
 * ��������TIM2_NVIC_Configuration
 * ����  ��TIM2�ж����ȼ�����
 * ����  ����
 * ���  ����	
 */
void spi2_dma_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;//SPI2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


//��ʼ��DMA   ע���ʱֻ�ǳ�ʼ��  ��û�п�ʼ����
uint8_t spi2_dma_init(void)
{
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
   DMA_Cmd(DMA1_Channel5, DISABLE);

  

   DMA_DeInit(DMA1_Channel5);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;      //���跢�����ݼĴ�����ַ
  
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                  //���ݴ��䷽��  Ϊ������Ŀ���ַ 
  
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;      //�����ַ�Ƿ�����  ��
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;               //�ڴ��ַ�Ƿ�����  ��
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //�����ǰ����ֽ�д��
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //�ڴ水���ֽڶ�ȡ
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             //����д��  ��ѭ��ģʽ
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;                       //���ȼ� ��
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                //�Ƿ�Ϊ�ڴ洫�ڴ�  ��
   DMA_Init(DMA1_Channel5, &DMA_InitStructure);

   DMA_ITConfig (DMA1_Channel5,DMA_IT_TC,ENABLE);

   


   return 0;
}

//��ʼ����DMA  ��Ҫ���õ��� �ַ����ĵ�ַ ����Ҫ�������Ŀ
uint8_t  start_dma_send(uint8_t *str_p ,uint16_t num)
{
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)str_p;              //�ڴ��ַ

    DMA_InitStructure.DMA_BufferSize = num;

    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
   /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel5, ENABLE);


   return 0;
}


