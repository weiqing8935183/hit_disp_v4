#include "DM163.H"
#include "init_spi.h"
#include "systick.h"
extern const unsigned char font8_8[][8] ;
                              // R   g   b



//定义点阵显示的结构体
MATRIX_DATA_INFO       matrix_data_str ;
//定义点阵屏欲发送的 队列
MATRIX_SEND_MSG_QUEUE  matrix_send_queue;
//点阵屏的控制结构体
MATRIX_CONTRAL_INFO   matrix_crl;

/****************************************************
          点阵屏硬件相关操作
          此部分是直接操作点阵屏的管脚
          主要为复位，显示使能 ，锁存等操作

****************************************************/


void matrix_rst(unsigned char temp)
{
  if (temp) 
    GPIOB->BSRR |= RST_BIT;
  else
    GPIOB->BRR |= RST_BIT;
}

void matrix_lat(unsigned char temp)
{
  if (temp) 
    GPIOB->BSRR |= LAT_BIT;
  else
    GPIOB->BRR |= LAT_BIT;
}

void matrix_slb(unsigned char temp)
{
  if (temp) 
    GPIOB->BSRR |= SLB_BIT;
  else
    GPIOB->BRR |= SLB_BIT;
}
void matrix_en(unsigned char temp)
{
  if (temp) 
    GPIOB->BSRR |= EN_BIT;
  else
    GPIOB->BRR |= EN_BIT;
}

//PC678 为行选择
//输入参数为0 - 7 
void light_matrix_row(uint8_t temp)
{
   if(temp>7)
   {
       return ;
   }
   else
   {
         GPIOC->BSRR |= temp<<6;       //设置对应的位为1
         GPIOC->BRR  |=((~temp)&(0x0007))<<6;   //清除对应的位为0
   }
}

void matrix_hal_init(void)
{
   uint32_t  i;
   pin_matrix_rst;            //rst脚  为低时候复位    为高的时候工作
   for(i=0;i<0xfffff;i++);
   pin_matrix_run;            //rst脚  为低时候复位    为高的时候工作

   pin_disable_matrix;
   pin_enable_matrix;         //en脚   为低的时候使能  为高的时候禁止

   matrix_lat(0);             //锁存脚一直保持为低  只有在需要锁存是在动一下 

   pin_select_fg;            //默认选择前景色 只有需要发送背景色时再选背景色 
 // SetGamma();

  
}



/********************************************************************************
     点阵屏数据结构体相关操作
    对于点阵屏定义了一个数据结构体

****************************************************************************/

//返回点阵结构体数组的头指针
MATRIX_DATA_INFO *get_matrix_data_p (void)
{

  return     &matrix_data_str ;
}

//初始化点阵屏的数据
uint8_t init_matrix_data_str(MATRIX_DATA_INFO * p)
{
    uint8_t i ,j,k;

    for(i=0;i<MATRIX_NUM;i++)
    {

      //设置第i个模块 的三个背景色
       p->bk_color[i][0]= 60;
       p->bk_color[i][1]= 60;
       p->bk_color[i][2]= 60;

       //将颜色扩展到对应的数组中去
       extra_bk_color(p->bk_color_str[i] ,  p->bk_color[i]);

      for(k=0;k<8;k++)
      {
          for(j=0;j<8;j++)
          {
              //设置第i个模块的 j 点的三个前景色
              p->fg_color_str[k][i][j][0] = 0;
              p->fg_color_str[k][i][j][1] = 0;
              p->fg_color_str[k][i][j][2] = 0;
          }
      }
    }


    return 0;
}

//将一个三个字节的背景色数组 扩展为18个字节的已模块数组
//原数组中是6位占用一个字节   目的是将每6个位中的多余两位删除，将整个数组紧缩后重拍
uint8_t extra_bk_color(uint8_t * des_p , uint8_t *src_p)
{
   uint8_t rtn=0;
   uint8_t i;

   uint8_t  temp_str[9];

   temp_str[0] = (*src_p<<2)|(*(src_p+1)>>4);
   temp_str[1] = (*(src_p+1)<<4)|(*(src_p+2)>>2);
   temp_str[2] = (*(src_p+2)<<6)|(*(src_p)&0x3f);

   temp_str[3] = (*(src_p+1)<<2)|(*(src_p+2)>>4);
   temp_str[4] = (*(src_p+2)<<4)|(*(src_p)>>2);
   temp_str[5] = (*(src_p)<<6)|(*(src_p+1)&0x3f);

   temp_str[6] =  (*(src_p+2)<<2)|(*(src_p)>>4);
   temp_str[7] =  (*(src_p)<<4)|(*(src_p+1)>>2);
   temp_str[8] =  (*(src_p+1)<<6)|(*(src_p+2)&0x3f);


   for(i=0;i<18;i++)
   {
        *(des_p+i)  =  temp_str[ i%9 ] ;
   }


   return rtn;

 

}

//返回背景数组的头地址
uint8_t * get_bsk_str_p(MATRIX_DATA_INFO * p)
{
       return (uint8_t *)p->bk_color_str ;
}
//返回前景色的
uint8_t * get_fg_str_p(MATRIX_DATA_INFO * p)
{
      return (uint8_t *)p->fg_color_str;
}

/*********************************************************************

点阵屏发送队列操作
对点阵屏每次刷新一行 
欲刷新的数据写入队列
此部分是针对队列的操作
*********************************************************************/
//返回队列的指针
MATRIX_SEND_MSG_QUEUE * get_matrix_queue_p(void)
{

   return   &matrix_send_queue;
}
//初始化队列
uint8_t  init_matrix_msg_queue(MATRIX_SEND_MSG_QUEUE * queue_p)
{
   uint8_t rtn =0;


   if(queue_p != 0)
   {
      queue_p->head = queue_p->tail = 0;
      
   }
   else
   {
       rtn =1 ;
   }

   return rtn ;
   
}
 //将一欲发送的数据帧插入到队列尾去
//queue_p  队列指针
//modbus_p 欲发送数据指针

uint8_t  insert_matrix_queue_tail(MATRIX_SEND_MSG_QUEUE* queue_p, MATRIX_SEND_MSG_INFO * msg_p )
{
      uint8_t  rtn = 0;
     

      if((queue_p->tail - queue_p->head + MATIRX_QUEUE_NUM)%MATIRX_QUEUE_NUM >= (MATIRX_QUEUE_NUM-2))  //当队列已满
      {
         rtn = 1;                                            //返回队列溢出
         /*********************************/
         spi2_dma_init();                              //初始化spi的dma
         init_matrix_msg_queue(queue_p);
         get_matrix_crl_p()->state = MATRIX_CRL_IDLE;
      }
      else
      {

         //将欲插入队列的数据复制
          queue_p->msg_str[queue_p->tail].type = msg_p->type; 

          queue_p->msg_str[queue_p->tail].num  = msg_p->num;

          queue_p->msg_str[queue_p->tail].str_p = msg_p->str_p;
           
         //队列尾加1
          queue_p->tail ++; 

          if(queue_p->tail == MATIRX_QUEUE_NUM )             //队尾是不是到了数组的尾部
          {
              queue_p->tail =  0;
          }


   

      }



      return rtn;
}


//删除队头的数据
uint8_t  delet_matrix_queue_head(MATRIX_SEND_MSG_QUEUE * queue_p)
{
  uint8_t  rtn =0;


  if(queue_p->head != queue_p->tail)    //当队头不等于对尾  即队列不为空
  {
       queue_p->head ++;                //将队头加一   即删去了队头

       if(queue_p->head >= MATIRX_QUEUE_NUM)     //如果队头已经指向了数组外面
       {
          queue_p->head = 0;              //将队头指向数组头
       }
   
  }
  else
  {
      rtn =1;
  }


   return rtn;
}


//得到队列头 的指针
MATRIX_SEND_MSG_INFO *  get_matrix_queue_head_p(MATRIX_SEND_MSG_QUEUE* queue_p)
{
    MATRIX_SEND_MSG_INFO * rtn_p;
    if(queue_p->tail == queue_p->head)
    {
      rtn_p =  0;
    }
    else
    {
       rtn_p = &(queue_p->msg_str[queue_p->head]);
    }


    return rtn_p ;
}
/*
 点阵屏控制部分结构体
*/

//返回点阵屏控制结构体指针
MATRIX_CONTRAL_INFO *get_matrix_crl_p(void)
{
    return &matrix_crl;
}

uint8_t start_matrix_row_send(MATRIX_CONTRAL_INFO * crl_p)
{
    uint8_t rtn=0;

    if(crl_p->state == MATRIX_CRL_IDLE)
    {
         crl_p->msg_p =   get_matrix_queue_head_p(get_matrix_queue_p()); //返回队列的头指针

         if(crl_p->msg_p != 0)      //头指针若不为0 开始发送
         {
              crl_p->state = MATRIX_CRL_BUSY;

             if((crl_p->msg_p->type) ==  BK_TYPE)
             {
                 pin_select_bk;                
             }
             else
             {
                 pin_select_fg;
             }
             start_dma_send(crl_p->msg_p->str_p ,crl_p->msg_p->num);

           
         }
         else          //头指针若为0 则队列中没有数据 返回1
         {
             rtn  =1;
         }
    }
    else             //若控制器正在忙
    {
        rtn =2;
    }



    return rtn ;
}
//发送中断函数
uint8_t end_matrix_row_send_irp(MATRIX_CONTRAL_INFO * crl_p)
{
      uint8_t rtn=0 ;

      if(crl_p->msg_p->type == BK_TYPE)     //若发送的是背景色 则引脚状态为前景色  ，
      {
          pin_select_fg;                    //引脚一般都是在前景色状态， 只有发送背景色时才改为背景色状态
      }


      pin_lat_action;                       //数据锁存


      light_matrix_row(get_matrix_data_p()->row_now);     //将显示切换到刚才发送的行

      delet_matrix_queue_head(get_matrix_queue_p());   //删除队列的头指针

      crl_p->state = MATRIX_CRL_IDLE;

      crl_p->msg_p =   get_matrix_queue_head_p(get_matrix_queue_p()); //返回队列的头指针

      

         if(crl_p->msg_p != 0)      //头指针若不为0 说明队列中还有数据 开始发送
         {
             if(crl_p->msg_p->type ==  BK_TYPE)
             {
               pin_select_bk;
             }
             crl_p->state = MATRIX_CRL_BUSY;

             

             start_dma_send(crl_p->msg_p->str_p ,crl_p->msg_p->num);
              ///////////////////////////
               
               
         }
         else          //头指针若为0 则队列中没有数据 返回1
         {
             
             rtn  =1;
         }


         return rtn ;
}

/********************************************/

uint8_t  insert_queue_bkg_persent(uint8_t per)
{                  
   uint32_t color;
   uint8_t  color_s,rtn = 0;

   if(per >=100)
   {
      color_s = 63; 
   }
   else
   {
     color_s = (per*63/100);
   }

   color = color_s<<16|color_s<<8|color_s;

   rtn =insert_queue_bkg(color);

   return rtn;
}
//向队列中插入刷新背景色的数组
uint8_t  insert_queue_bkg(uint32_t color)
{
    uint8_t rtn =0;
    MATRIX_SEND_MSG_INFO  msg;
    uint8_t i;
       for(i=0;i<MATRIX_NUM;i++)
       {
          //设置第i个模块 的三个背景色
          get_matrix_data_p()->bk_color[i][0]= color;
          get_matrix_data_p()->bk_color[i][1]= color>>8;
          get_matrix_data_p()->bk_color[i][2]= color>>16;

           //将颜色扩展到对应的数组中去
           extra_bk_color(get_matrix_data_p()->bk_color_str[i] ,  get_matrix_data_p()->bk_color[i]);
      }


    msg.type  = BK_TYPE;
    msg.num   = BK_NUM;
    msg.str_p = (uint8_t *)get_matrix_data_p()->bk_color_str;
    rtn       = insert_matrix_queue_tail(get_matrix_queue_p(), &msg ) ;
    rtn      += start_matrix_row_send(get_matrix_crl_p());
   return   rtn;
}
//  向队里中插入刷新前景 的数组  
uint8_t  insert_queue_fg(uint8_t row)
{
    uint8_t rtn =0;
    MATRIX_SEND_MSG_INFO  msg;
   if(row <8)
   {
    msg.type  = FG_TYPE;
    msg.num   = FG_NUM;
    msg.str_p = (uint8_t *)get_matrix_data_p()->fg_color_str[row];
    rtn       = insert_matrix_queue_tail(get_matrix_queue_p(), &msg ) ;
    rtn      += start_matrix_row_send(get_matrix_crl_p());

    }
    else
    {
        rtn =0xff;
    }
   return   rtn;
}

//插入刷新前景色一行 的钩子函数  
uint8_t  refresh_matrix_row_hook(void * p)
{
    //uint8_t rtn = 0 ;

    MATRIX_DATA_INFO * data_p;

    data_p = p ;

    data_p->row_now ++ ;

    if(data_p->row_now >=8)
    {
       data_p->row_now = 0;
    }

   insert_queue_fg(data_p->row_now);

   //creat_timer((void *) get_matrix_data_p(),1 ,refresh_matrix_row_hook);

   refresh_timer(get_matrix_data_p()->tm_p);

  return  0 ;
}










