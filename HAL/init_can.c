              #include "init_can.h"
/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN的GPIO 配置,PB8上拉输入，PB9推挽输出
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_GPIO_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
   	
  	/*外设时钟设置*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

  	/*IO设置*/
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	/* Configure CAN pin: RX */									               // PB8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Configure CAN pin: TX */									               // PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

/*
 * 函数名：CAN_NVIC_Config
 * 描述  ：CAN的NVIC 配置,第1优先级组，0，0优先级
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
		/* Configure one bit for preemption priority */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	 	/*中断设置*/
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	   //CAN1 RX0中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;		   //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    	 	/*发送中断设置*/
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;	   //CAN1 TX0中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =4;		   //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			   //子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}


/*
 * 函数名：CAN_Mode_Config
 * 描述  ：CAN的模式 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Mode_Config(void)
{
    	CAN_InitTypeDef        CAN_InitStructure;
	 	/************************CAN通信参数设置**********************************/
	/*CAN寄存器初始化*/
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	/*CAN单元初始化*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  关闭时间触发通信模式使能
    CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  自动离线管理 
    CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  使用自动唤醒模式
    CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
    CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
    CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //正常工作模式
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW 重新同步跳跃宽度 1个时间单元
    CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;		   //BTR-TS1 时间段1 占用了16个时间单元
    CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 时间段2 占用了7个时间单元
    CAN_InitStructure.CAN_Prescaler =4;		   ////BTR-BRP 波特率分频器  定义了时间单元的时间长度 36/(1+6+3)/4=0.8Mbps
	CAN_Init(CAN1, &CAN_InitStructure);
	//注意在STM32中的CAN_InitStructure.CAN_Prescaler和BRP5-0的意义一样，是对输入时钟进行分频
	//比如在SJA1000中BRP5-0为7，则CAN_Prescaler应为36，
	//因为SJA1000的输入频率为16M  BRP的分频方式为2*(BRP+1)分频 ，而STM32对CAN输入频率为36Mhz ，直接分频 

	//在SJA1000  针对18M晶振下的波特率设置	
	//波特率设置对照表 BTR0  BTR1		  BTR0(BIT7-6)	  BTR0(BIT5-0)	 BTR1(BIT7)	   BTR1(BIT6-4)		    BTR1(BIT3-0)
	//									  SJW.1 SJW.0	  BRP.5-BRP.0		SAM		   TESG2.2-TESG2.0		TESG1.3-TESG1.0
	//	1m             0x00 0x15				0				0			 0				1					 5
	//500K             0X00 0X1E				0				0			 0				1					 E
	//250K  		   0X01	0X1E				0				1			 0				1					 E
	//125K             0X03 0X1E			    0				3			 0				1					 E
	//100K             0X04 0X1E				0				4			 0				1					 E
    //50K              0X08 0X2F				0				8			 0				2					 F


	//在SJA1000  针对16M晶振下的波特率设置	
	//波特率设置对照表 BTR0  BTR1		  BTR0(BIT7-6)	  BTR0(BIT5-0)	 BTR1(BIT7)	   BTR1(BIT6-4)		    BTR1(BIT3-0)
	//									  SJW.1 SJW.0	  BRP.5-BRP.0		SAM		   TESG2.2-TESG2.0		TESG1.3-TESG1.0
	//	1m             0x00 0x14				0				0			 0				1					 4
	//  800K           0X00 0X16				0				0			 0				1					 6
	//500K             0X00 0X1C				0				0			 0				1					 C
	//250K  		   0X01	0X1C				0				1			 0				1					 C
	//125K             0X03 0X1C			    0				3			 0				1					 C
	//100K             0X04 0X1C				0				4			 0				1					 C
    //50K               0X09 0X1C				0				9			 0				1					 C
    //40K               0X07 0XFF				0				7			 1				7					 F

}

/*
 * 函数名：CAN_Filter_Config
 * 描述  ：CAN的过滤器 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Filter_Config(void)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN过滤器初始化*/
	CAN_FilterInitStructure.CAN_FilterNumber=0;						//过滤器组0
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//工作在标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//过滤器位宽为单个32位。
	/* 使能报文标示符过滤器按照标示符的内容进行比对过滤，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */
	     
		/*bit15 ----bit8      bit7-bit5    bit4    bit3       bit2-0  */
		/* stdid10---3         stdid3-0    rtr     ide       extid17-15 */
		/*32位屏蔽码和掩码模式下的定义*/
		/*bit31 ----bit24   bit23-bit21    bit20-bit16    bit15-bit8   bit7-bit3  bit2  bit1  bit0*/
		/*    stdid10-3       stdid2-0     extid17-13     extid12-5    extid4-0   ide   rtr    0  */
		
    CAN_FilterInitStructure.CAN_FilterIdHigh=0;				//		要过滤的ID位
    CAN_FilterInitStructure.CAN_FilterIdLow=4;                // 要过滤的ID位
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0X0;		//要过滤的ID高位 需要屏蔽的位 0 不关心，1 必须匹配
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0X04;			//  要过滤的ID高位 需要屏蔽的位 0 不关心，1 必须匹配
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//过滤器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CAN通信中断使能*/
	CAN_ITConfig(CAN1, CAN_IT_FMP0|CAN_IT_TME, ENABLE);
}


/*
 * 函数名：CAN_Config
 * 描述  ：完整配置CAN的功能
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void ican_config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();   
}


