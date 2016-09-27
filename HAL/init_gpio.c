#include "init_gpio.h"



void init_gpio(void)
{
    init_addr_gpio();
    init_key_gpio();
    init_dm163_gpio();
    init_led_gpio();
}


//初始化拨码开关的的gpio    为PD8-PD15
void init_addr_gpio(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOC的外设时钟*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
	/*初始化D口引脚**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	
}





//返回地址函数
uint8_t  get_addr(void)
{
  uint16_t rtn;

  rtn =  ~GPIO_ReadInputData(GPIOD);

  return rtn>>8;
}




//初始化按键输入的引脚  PE8-PE15
void init_key_gpio(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOC的外设时钟*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
	/*初始化E口引脚**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
  	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}


//初始化DM163的引脚
//PC6  PC7 PC8   为行选择输出 接入138   

//PB10    DM163  ---- RST       (低有效)
//PB11    DM163  ---- LAT       ()
//PB12    DM163  ---- SECLET
//PB13    DM163  ---- DCK
//PB14    DM163  ---- EN
//PB15    DM163  ---- SIN
void init_dm163_gpio(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOC的外设时钟*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
//	/*初始化B口引脚**/														   
//  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//  	GPIO_Init(GPIOB, &GPIO_InitStructure);	



    	/*初始化c口引脚**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}



//初始化LED输出引脚 
//为PD0 - PD7
void init_led_gpio(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOC的外设时钟*/
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);


														   
	
	/*初始化B口引脚**/														   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	



	
}
