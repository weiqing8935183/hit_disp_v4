#include "init_mcp2515.H"
#include "init_gpio.h"


/****************************************************************************
*������ OutCANInit
*����   ��ʼ���ⲿCAN ����
*       
*         

*
*
*
*���ߣ� κ��
*���ڣ� 14.12.24
******************************************************************************/
void  out_can_config(void)
{

   uint8_t add;
   add=get_addr();
   SPIPort_Init();                              //��ʼ��SPI�˿�  ����IO�ĳ�ʼ����SPI�ڵĳ�ʼ��

   MCP2515_RESET();                             //����MCP2515��
   MCP2515_ENTERCONFIG();                       //2515��������ģʽ
   MCP2515_Init();                              //����2515�Ĳ�����
   MCP2515_Filter_Set(0,1,add<<16);
   MCP2515_Mask_Set(0,1,0xffff0000);
   MCP2515_Mask_Set(1,1,0xffffffff);
   MCP2515_IT_Enable(0x03);
   MCP2515_ENTERWORK();

   EXTI_PC4_Config();
   EXTI_PC4_NVIC_Config();

}

void EXTI_PC4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
   

	/* config the extiline(PA8) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);

	/* EXTI line gpio config(PA8) PA8��ΪMCP2515���ж�����*/	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // ��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* EXTI line(PA8) mode config */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 
}


void EXTI_PC4_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
    /* nvic�жϴ� */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
}

/*******************************************************************************
* Function Name  : SPIPort_Init
* Description    : SPI�˿ڳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPIPort_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);

  /*!< SPI_FLASH_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
 
  
  /*!< Configure SPI_FLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


   /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
  GPIO_SetBits(GPIOA, GPIO_Pin_3);

  /* SPI1 configuration */
  // W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
  // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : ��SPI�ڷ���һ���ֽ�,�˺���ֱ�Ӷ�SPI�ڲ�����û������cs��
*                  
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
uint8_t  SPIPort_SendByte(uint8_t  byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}
/*******************************************************************************
* Function Name  : SPIPort_ReadByte
* Description    : ��SPI�ڶ�һ���ֽ� �˺���ֱ�Ӷ�SPI�ڲ�����û������cs��
*                  
*                  
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
uint8_t SPIPort_ReadByte(void)
{
  return (SPIPort_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : MP2515_Write
* Description    : ��MCP2515дһ���ֽ�
*                  ��һ���ֽ�0x02Ϊд����   �ڶ����ֽ�Ϊ��д��ĵ�ַ  �������ֽ�Ϊ��д�������
*                  
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
void MCP2515_Write(uint8_t addr,uint8_t byte)
{
  SPI_FLASH_CS_LOW();

  SPIPort_SendByte(0x02);
  SPIPort_SendByte(addr);
  SPIPort_SendByte(byte);

  SPI_FLASH_CS_HIGH();   
}
/*******************************************************************************
* Function Name  : MP2515_Write
* Description    : ��MCP2515��һ���ֽ�
*                   ��һ���ֽ�0x02Ϊ������   �ڶ����ֽ�Ϊ�����ĵ�ַ  �������ֽ�Ϊ������������
*                  
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
uint8_t MCP2515_Read(uint8_t addr)
{
  uint8_t temp;
  SPI_FLASH_CS_LOW();

  SPIPort_SendByte(0x03);
  SPIPort_SendByte(addr);
  temp=SPIPort_ReadByte();

  SPI_FLASH_CS_HIGH();   

  return temp;
}
/*******************************************************************************
* Function Name  : SPI_MP2515_RESET
* Description    : ����2515
*                  
*                  
* Input          : None
* Output         : None
* Return         : 0 �����ɹ�    1����ʧ��
*******************************************************************************/
uint8_t MCP2515_RESET(void)
{

 uint8_t  num=0 ,temp=0;
    /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPIPort_SendByte(MCP2515_CMD_RESET);


  while((num<10)&&(temp!=0x80)) 
  {           
    SPIPort_SendByte(MCP2515_CMD_READ_STATUE);
    temp=SPIPort_ReadByte();
    num++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();  
  
  if(temp==0x80) 
  {
   return 0;
  }
  else
  {
    return 1;
  }
}  
/*******************************************************************************
* Function Name  : SPI_MP2515_READ_STATUE
* Description    : ��ȡ2515��״̬�Ĵ���
*                  
*                  
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
uint8_t MCP2515_READ_STATUE(void)
{
  uint8_t temp;
    /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPIPort_SendByte(MCP2515_CMD_READ_STATUE);
  temp=SPIPort_ReadByte();


  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();  
  
  return temp; 
}  

/*******************************************************************************
* Function Name  : MP2515_Init
* Description    : 2515�ĳ�ʼ��
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void MCP2515_Init(void)
{
      MCP2515_Write(0x2a,0x01);                       // sjw=0   bpr=1  7-6:sjw   5-0:bpr    cnf1


      MCP2515_Write(0x29,0xff);                       //CNF2
      // BTLMODE      SAM                            PHSEG12  PHSEG11  PHSEG10      PRSEG2 PRSEG1 PRSEG0  
      //              �������� 0:1��    1:3��        (PSEG1+1)*TQ��λ�����1        (PRSEG+1)*TQ  �����γ���      
      
                                                        
      MCP2515_Write(0x28,0x07);                       //                                     cnf3
      //SOF                         WAKFIL                      UNSED   UNSED  UNSED   PHSEG22   PHSEG21  PHSEG20
      //1 clkoutΪsof�ź�           1�����˲���ʹ��                                   (PHSEG2+1)*2 Ϊ��λ�����2����
      //0 clkoutΪΪʱ�����
}
/*******************************************************************************
* Function Name  : SPI_MP2515_ENTERWORK
* Description    : 2515���빤��״̬
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void  MCP2515_ENTERWORK(void)
{
  MCP2515_Write(0x0f,0x06); 
}
/*******************************************************************************
* Function Name  : SPI_MP2515_ENTERCONFIG
* Description    : 2515��������״̬
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void  MCP2515_ENTERCONFIG(void)
{
  MCP2515_Write(0x0f,0x84); 
}
/*******************************************************************************
* Function Name  : MCP2515_Filter_Set
* Description    : �����˲��Ĵ���
*                  
*                  
* Input          : chanelָʾ�ĸ��˲��Ĵ��� 0-5    IDEָʾ����չ�ͻ��Ǳ�׼�� 1��չ��  0��׼��  
* Output         : None
* Return         :
*******************************************************************************/
void MCP2515_Filter_Set(uint8_t chanel,uint8_t IDE,uint32_t ID)     //�����˲��Ĵ�����
{
  uint8_t offset;
  uint8_t temp;

  offset=chanel*0x04;

     if(IDE==1)
     {
      temp=ID>>21;
      MCP2515_Write(0x0+offset,temp);                        //sid 10-3
      temp=(((ID)>>13)&0XE0)|(((ID)>>16)&0X03)|0X08;
      MCP2515_Write(1+offset,temp);          //sid 2-0   exide   eid 17-16
      temp=((ID)>>8)&0X000000FF;
      MCP2515_Write(2+offset,temp);
      temp=((ID))&0X000000FF;  
      MCP2515_Write(3+offset,temp); 
      }
      else
      {
       temp=(ID)>>3;
       MCP2515_Write(0+offset,temp);                        //sid 10-3
       temp=(((ID)&0X00000007)<<5);
       MCP2515_Write(1+offset,temp);                        //sid 2-0   exide   eid 17-16
      }


   
}
/*******************************************************************************
* Function Name  : MCP2515_Mask_Set
* Description    : �������μĴ���
*                  
*                  
* Input          : chanelָʾ�ĸ����μĴ��� 0-1   IDEָʾ�Ƿ�Ϊ��չ֡  IDΪ֡��
* Output         : None
* Return         :
*******************************************************************************/
void MCP2515_Mask_Set(uint8_t chanel,uint8_t IDE,uint32_t ID)         //ֻ���������μĴ�����
{
  uint8_t offset;
  uint8_t temp;

  offset=chanel*0x04;

     if(IDE==1)
     {
          temp=ID>>21;
          MCP2515_Write(0x20+offset,temp);                        //sid 10-3
          temp=(((ID)>>13)&0XE0)|(((ID)>>16)&0X03);
          MCP2515_Write(0x21+offset,temp);          //sid 2-0   exide   eid 17-16
          temp=((ID)>>8)&0X000000FF;
          MCP2515_Write(0x22+offset,temp);
          temp=((ID))&0X000000FF;  
          MCP2515_Write(0x23+offset,temp); 
      }
      else
      {
          temp=(ID)>>3;
          MCP2515_Write(0x20+offset,temp);                        //sid 10-3
          temp=(((ID)&0X00000007)<<5);
          MCP2515_Write(0x21+offset,temp);                        //sid 2-0   eid 17-16
          MCP2515_Write(0x22+offset,0x00);
          MCP2515_Write(0x23+offset,0x00);
      }

}

/*******************************************************************************
* Function Name  : MCP2515_IT_Clear
* Description    : ���2515���ж�
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void MCP2515_IT_Clear(uint8_t i)
{
   SPI_FLASH_CS_LOW();

  
  SPIPort_SendByte(0x05);       // λ�޸�ָ��
  SPIPort_SendByte(0x2c);       //��ַ
  SPIPort_SendByte(i);          //�����ֽ�
  SPIPort_SendByte(0);          //�����ֽ�

  SPI_FLASH_CS_HIGH(); 

}
/*******************************************************************************
* Function Name  : MCP2515_IT_Enable
* Description    : ������Щ�ж�
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void MCP2515_IT_Enable(uint8_t i)
{
  SPI_FLASH_CS_LOW();

  
  SPIPort_SendByte(0x05);       // λ�޸�ָ��
  SPIPort_SendByte(0x2b);       //��ַ
  SPIPort_SendByte(i);          //�����ֽ�
  SPIPort_SendByte(0xff);          //�����ֽ�

  SPI_FLASH_CS_HIGH(); 
}
/*******************************************************************************
* Function Name  : MCP2515_IT_Get
* Description    : ��ȡ2515���ж�
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
uint8_t MCP2515_IT_Get(void)
{
    return MCP2515_Read(0x2c);
}


/*******************************************************************************
* Function Name  : MPC2515_CAN_SEND
* Description    : ��2515����һ��CAN���� ������ TxMessage�ṹ����
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void MPC2515_CAN_SEND(CanTxMsg* TxMessage)
{
    uint8_t temp;
    uint8_t offset=0;  //��ַƫ����

    temp=MCP2515_Read(0x30);
    if((temp&0x08)==0)
    {
       offset=0;
    }
    else if((MCP2515_Read(0x40)&0x08)==0)
    {
      offset=0x10;
    }
    else if((MCP2515_Read(0x50)&0x08)==0)
    {
       offset=0x20;
    }
    else
    {
       return ;
    }


    if (TxMessage->IDE == CAN_Id_Standard)
    {
      temp=(TxMessage->StdId)>>3;
      MCP2515_Write(0x31+offset,temp);                        //sid 10-3
      temp=(((TxMessage->StdId)&0X00000007)<<5);
      MCP2515_Write(0x32+offset,temp);                        //sid 2-0   exide   eid 17-16
     
    }
    else
    {
       temp=(TxMessage->ExtId)>>21;
       MCP2515_Write(0x31+offset,temp);                        //sid 10-3
       temp=(((TxMessage->ExtId)>>13)&0XE0)|(((TxMessage->ExtId)>>16)&0X03)|0X08;
       MCP2515_Write(0x32+offset,temp);          //sid 2-0   exide   eid 17-16
       temp=((TxMessage->ExtId)>>8)&0X000000FF;
       MCP2515_Write(0x33+offset,temp);
       temp=((TxMessage->ExtId))&0X000000FF;  
       MCP2515_Write(0x34+offset,temp);   

    }
      MCP2515_Write(0x30+offset,0x00);                        //TXB0CTRL

     if(TxMessage->RTR==CAN_RTR_REMOTE)
     {
        MCP2515_Write(0x35+offset,0x40|TxMessage->DLC);                        //rtr   dlc 
     }
     else
     {
       MCP2515_Write(0x35+offset,TxMessage->DLC);                        //rtr   dlc 
     }
     
      MCP2515_Write(0x36+offset,TxMessage->Data[0]);                        //DATA1
      MCP2515_Write(0x37+offset,TxMessage->Data[1]);                        //DATA2
      MCP2515_Write(0x38+offset,TxMessage->Data[2]);                        //DATA3
      MCP2515_Write(0x39+offset,TxMessage->Data[3]);                        //DATA4
      MCP2515_Write(0x3A+offset,TxMessage->Data[4]);                        //DATA5
      MCP2515_Write(0x3B+offset,TxMessage->Data[5]);                        //DATA6
      MCP2515_Write(0x3C+offset,TxMessage->Data[6]);                        //DATA7
      MCP2515_Write(0x3D+offset,TxMessage->Data[7]);                        //DATA8
      MCP2515_Write(0x30+offset,0x08); 
}

/*******************************************************************************
* Function Name  : MPC2515_CAN_SEND
* Description    : ��2515��ȡһ��CAN���� ������ RxMessage�ṹ����
*                  
*                  
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
void MCP2515_CAN_Receive(CanRxMsg *RxMessage)
{
    uint8_t temp;
    uint8_t tempdlc;
    uint8_t offset;

   
              temp=MCP2515_Read(0x2c);  //��ȡ�жϼĴ���
              if((temp&0x01)!=0)
              {
                  offset=0;
                  RxMessage->FMI=1;
              }
              else if((temp&0x02)!=0)
              {
                offset=0x10;
                RxMessage->FMI=2;
              }
              else
              {
                RxMessage->FMI=0;
                return;
              }

              temp=MCP2515_Read(0x62+offset); //sid2-0 ssr ide eid17-eid16
              if((temp&0x08)!=0)        //������0Ϊ��չ֡
              {
                RxMessage->ExtId= (((MCP2515_Read(0x61+offset)<<3)|(temp>>5))<<18)
                                  |((temp&0x03)<<16)
                                  |((MCP2515_Read(0x63+offset)<<8))
                                  |(MCP2515_Read(0x64+offset));
                RxMessage->IDE=CAN_ID_EXT;
                temp=MCP2515_Read(0x65+offset);
                if((temp&0x40)!=0) //��չ֡��Զ��֡
                {
                    RxMessage->RTR=CAN_RTR_REMOTE;
                }
                else
                {
                   RxMessage->RTR=CAN_RTR_DATA;
                }
                RxMessage->DLC=temp&0x0f;
              }
              else
              {
                  
                 RxMessage->StdId=(MCP2515_Read(0x61+offset)<<3)|(temp>>5);
                 RxMessage->IDE=CAN_ID_STD;
                 if((temp&0x10)!=0)    //��׼֡��Զ��֡
                 {
                   RxMessage->RTR=CAN_RTR_REMOTE;
                 }
                 else
                 {
                   RxMessage->RTR=CAN_RTR_DATA;
                 }
                 temp=MCP2515_Read(0x65+offset);
                 RxMessage->DLC=temp&0x0f;
              }

              if(RxMessage->DLC<=8)
              {
                tempdlc=RxMessage->DLC;
              }
              else
              {
                tempdlc=8;
              }

              for(temp=0;temp<tempdlc;temp++)
              {
                 RxMessage->Data[temp]= MCP2515_Read(0x66+temp+offset);
              }

             MCP2515_Write(0x2c,0);     //����жϱ�־
}
