#include "init_gpio.h"



void init_gpio(void)
{
    init_addr_gpio();
    init_key_gpio();
    init_dm163_gpio();
    init_led_gpio();
}


//��ʼ�����뿪�صĵ�gpio    ΪPD8-PD15
void init_addr_gpio(void)
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOC������ʱ��*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
	/*��ʼ��D������**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	
}





//���ص�ַ����
uint8_t  get_addr(void)
{
  uint16_t rtn;

  rtn =  ~GPIO_ReadInputData(GPIOD);

  return rtn>>8;
}




//��ʼ���������������  PE8-PE15
void init_key_gpio(void)
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOC������ʱ��*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
	/*��ʼ��E������**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
  	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}


//��ʼ��DM163������
//PC6  PC7 PC8   Ϊ��ѡ����� ����138   

//PB10    DM163  ---- RST       (����Ч)
//PB11    DM163  ---- LAT       ()
//PB12    DM163  ---- SECLET
//PB13    DM163  ---- DCK
//PB14    DM163  ---- EN
//PB15    DM163  ---- SIN
void init_dm163_gpio(void)
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOC������ʱ��*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
//	/*��ʼ��B������**/														   
//  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//  	GPIO_Init(GPIOB, &GPIO_InitStructure);	



    	/*��ʼ��c������**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}



//��ʼ��LED������� 
//ΪPD0 - PD7
void init_led_gpio(void)
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOC������ʱ��*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
	/*��ʼ��B������**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	



	
}
