              #include "init_can.h"
/*
 * ��������CAN_GPIO_Config
 * ����  ��CAN��GPIO ����,PB8�������룬PB9�������
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_GPIO_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
   	
  	/*����ʱ������*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

  	/*IO����*/
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
	/* Configure CAN pin: RX */									               // PB8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // ��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Configure CAN pin: TX */									               // PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

/*
 * ��������CAN_NVIC_Config
 * ����  ��CAN��NVIC ����,��1���ȼ��飬0��0���ȼ�
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_NVIC_Config(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure;
		/* Configure one bit for preemption priority */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	 	/*�ж�����*/
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	   //CAN1 RX0�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;		   //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //�����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    	 	/*�����ж�����*/
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;	   //CAN1 TX0�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =4;		   //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			   //�����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}


/*
 * ��������CAN_Mode_Config
 * ����  ��CAN��ģʽ ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Mode_Config(void)
{
    	CAN_InitTypeDef        CAN_InitStructure;
	 	/************************CANͨ�Ų�������**********************************/
	/*CAN�Ĵ�����ʼ��*/
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	/*CAN��Ԫ��ʼ��*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
    CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  �Զ����߹��� 
    CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
    CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
    CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
    CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //��������ģʽ
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW ����ͬ����Ծ��� 1��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;		   //BTR-TS1 ʱ���1 ռ����16��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 ʱ���2 ռ����7��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_Prescaler =4;		   ////BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 36/(1+6+3)/4=0.8Mbps
	CAN_Init(CAN1, &CAN_InitStructure);
	//ע����STM32�е�CAN_InitStructure.CAN_Prescaler��BRP5-0������һ�����Ƕ�����ʱ�ӽ��з�Ƶ
	//������SJA1000��BRP5-0Ϊ7����CAN_PrescalerӦΪ36��
	//��ΪSJA1000������Ƶ��Ϊ16M  BRP�ķ�Ƶ��ʽΪ2*(BRP+1)��Ƶ ����STM32��CAN����Ƶ��Ϊ36Mhz ��ֱ�ӷ�Ƶ 

	//��SJA1000  ���18M�����µĲ���������	
	//���������ö��ձ� BTR0  BTR1		  BTR0(BIT7-6)	  BTR0(BIT5-0)	 BTR1(BIT7)	   BTR1(BIT6-4)		    BTR1(BIT3-0)
	//									  SJW.1 SJW.0	  BRP.5-BRP.0		SAM		   TESG2.2-TESG2.0		TESG1.3-TESG1.0
	//	1m             0x00 0x15				0				0			 0				1					 5
	//500K             0X00 0X1E				0				0			 0				1					 E
	//250K  		   0X01	0X1E				0				1			 0				1					 E
	//125K             0X03 0X1E			    0				3			 0				1					 E
	//100K             0X04 0X1E				0				4			 0				1					 E
    //50K              0X08 0X2F				0				8			 0				2					 F


	//��SJA1000  ���16M�����µĲ���������	
	//���������ö��ձ� BTR0  BTR1		  BTR0(BIT7-6)	  BTR0(BIT5-0)	 BTR1(BIT7)	   BTR1(BIT6-4)		    BTR1(BIT3-0)
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
 * ��������CAN_Filter_Config
 * ����  ��CAN�Ĺ����� ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Filter_Config(void)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN��������ʼ��*/
	CAN_FilterInitStructure.CAN_FilterNumber=0;						//��������0
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//�����ڱ�ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//������λ��Ϊ����32λ��
	/* ʹ�ܱ��ı�ʾ�����������ձ�ʾ�������ݽ��бȶԹ��ˣ���չID�������µľ����������ǵĻ��������FIFO0�� */
	     
		/*bit15 ----bit8      bit7-bit5    bit4    bit3       bit2-0  */
		/* stdid10---3         stdid3-0    rtr     ide       extid17-15 */
		/*32λ�����������ģʽ�µĶ���*/
		/*bit31 ----bit24   bit23-bit21    bit20-bit16    bit15-bit8   bit7-bit3  bit2  bit1  bit0*/
		/*    stdid10-3       stdid2-0     extid17-13     extid12-5    extid4-0   ide   rtr    0  */
		
    CAN_FilterInitStructure.CAN_FilterIdHigh=0;				//		Ҫ���˵�IDλ
    CAN_FilterInitStructure.CAN_FilterIdLow=4;                // Ҫ���˵�IDλ
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0X0;		//Ҫ���˵�ID��λ ��Ҫ���ε�λ 0 �����ģ�1 ����ƥ��
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0X04;			//  Ҫ���˵�ID��λ ��Ҫ���ε�λ 0 �����ģ�1 ����ƥ��
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;				//��������������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//ʹ�ܹ�����
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CANͨ���ж�ʹ��*/
	CAN_ITConfig(CAN1, CAN_IT_FMP0|CAN_IT_TME, ENABLE);
}


/*
 * ��������CAN_Config
 * ����  ����������CAN�Ĺ���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void ican_config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();   
}


