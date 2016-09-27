
#include "modbus_host.h"
#include "math.h"




//定义modbus的帧队列
MODBUS_QUEUE_INFO   modbus_queue;


//定义MODBUS 控制器的结构体
MODBUS_CONTRAL_INFO modbus_contral;
/*
     得到modbus队列数据结构的指针
*/
MODBUS_QUEUE_INFO * get_modbus_queue_p(void)
{
    return &modbus_queue;
}

//初始化队列结构体
uint8_t  init_modbus_queue(MODBUS_QUEUE_INFO* queue_p)
{
   uint8_t rtn ;


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
//将一欲发送的数据帧插入到队列中去
//queue_p  队列指针
//modbus_p 欲发送数据指针

uint8_t  insert_mod_queue_tail(MODBUS_QUEUE_INFO* queue_p, MODBUS_FRAME_INOF * modbus_p )
{
      uint8_t  rtn = 0;
      uint8_t  i;

      if((queue_p->tail - queue_p->head + QUEUE_NUM)%QUEUE_NUM >= (QUEUE_NUM-2))  //当队列已满
      {
         rtn = 1;                                            //返回队列溢出
      }
      else
      {

         //将发送部分的数据复制
          queue_p->frame_str[queue_p->tail].need_send_num = modbus_p->need_send_num; 

          queue_p->frame_str[queue_p->tail].have_send_num=0;

          for(i=0;i<modbus_p->need_send_num;i++)
          {
              queue_p->frame_str[queue_p->tail].send_str[i] =modbus_p->send_str[i] ;
          }
         //将接受部分的数据复制
           queue_p->frame_str[queue_p->tail].rec_stru_p = modbus_p->rec_stru_p;
           
         //队列加1
          queue_p->tail ++; 

          if(queue_p->tail == QUEUE_NUM )             //队尾是不是到了数组的尾部
          {
              queue_p->tail =  0;
          }


          //若控制结构体为空闲
          //发送头帧
          //此处不再开始发送 而是通过手动吊桶程序start_modbus_queue_send 发送
         //start_modbus_queue_send(queue_p,modbus_get_cm_p());


      }



      return rtn;
}


//删除队头的数据
uint8_t  delet_mod_queue_head(MODBUS_QUEUE_INFO* queue_p)
{
  uint8_t  rtn =0;


  if(queue_p->head != queue_p->tail)    //当队头不等于对尾  即队列不为空
  {
       queue_p->head ++;                //将队头加一   即删去了队头

       if(queue_p->head >= QUEUE_NUM)     //如果队头已经指向了数组外面
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
MODBUS_FRAME_INOF *  get_mod_queue_head_p(MODBUS_QUEUE_INFO* queue_p)
{
    MODBUS_FRAME_INOF * rtn_p;
    if(queue_p->tail == queue_p->head)
    {
      rtn_p =  0;
    }
    else
    {
       rtn_p = &(queue_p->frame_str[queue_p->head]);
    }


    return rtn_p ;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//得到控制结构体的指针
MODBUS_CONTRAL_INFO * modbus_get_cm_p( void)
{
    return  &modbus_contral;
}



//开始发送队列头的第一个字节 并对控制结构体置值
uint8_t  start_modbus_queue_send(MODBUS_QUEUE_INFO* queue_p,MODBUS_CONTRAL_INFO *mc_p)
{
     uint8_t rtn=0;
         if((mc_p->state==MB_IDLE)&&(queue_p->head != queue_p->tail))
          {
              //将队列的头置给控制结构体
              mc_p->mf_p  = &queue_p->frame_str[queue_p->head];
              
              mc_p->tm_p  = 0;            //无定时器


              modbus_hal_set_send();                 //置发送管脚
             
              //注意此处的操作顺序
              //先读状态寄存器 ，在写数据寄存器才可清除USART_IT_TC标志
              //然后才能开中断，不然的话中断开早了 会一直进入中断并死循环
              USART_GetITStatus( USART3, USART_IT_TC );
              USART_SendData(USART3, *(modbus_contral.mf_p->send_str+modbus_contral.mf_p->have_send_num));	 //发送数据
              mc_p->mf_p->have_send_num++;
               mc_p->state = MB_SEND;
              USART_ITConfig  (USART3,USART_IT_TC,ENABLE) ; //打开发送中断
             
                
          }
          else
          {
              rtn =1;
               //mc_p->tm_p  = 0;            //无定时器
          }


          return rtn;
}
//modbus send irq
//modbus 的字节发送成功 中断

uint8_t  modbus_send_irq(MODBUS_CONTRAL_INFO * cm_p)
{
  uint8_t rtn;
  if(cm_p->state ==  MB_SEND)
  {
     

      if(cm_p->mf_p->have_send_num < cm_p->mf_p->need_send_num)  //如果数据没有发送完
      {
             USART_SendData(USART3, *(cm_p->mf_p->send_str+cm_p->mf_p->have_send_num));	 //发送数据
             cm_p->mf_p->have_send_num ++;
      }
      else                                                              //如果数据已经发送完毕
      {
             cm_p->state = MB_SEND_END;                              //改变控制器状态 进发送完毕
             USART_ITConfig  (USART3,USART_IT_TC,DISABLE) ;          //关闭发送中断

            
             cm_p->tm_p=creat_timer(modbus_get_cm_p(),5,modbus_timeout_hook); //创建一个700us 4T秒的定时器 ，此定时器的作用是改变发送状态
             //此处应该打开定时器
      }
  }


  return rtn;
}


uint8_t modbus_receive_irq(MODBUS_CONTRAL_INFO * cm_p)
{
     uint8_t rtn=0;
  

      if((cm_p->state == MB_REC)||(cm_p->state == MB_WAIT_REC))
      {
          cm_p->state = MB_REC;
          change_timer(cm_p->tm_p,5);                                 //创建一1.5T间隔的定时器  为了检测字节间接受的的间隔不能大于1.5T
                                                                      //实际使用的时间是3T约510us  因为加上数据传送的时间间隔最大可达到2.5T
          cm_p->mf_p->rec_stru_p->state = REC_BUSY;
          cm_p->mf_p->rec_stru_p->rec_str[cm_p->mf_p->rec_stru_p->rec_num]   =   USART_ReceiveData(USART3);
          cm_p->mf_p->rec_stru_p->rec_num ++;
        

      }
      else                           //如果没有进入接受状态 则读一下寄存器，清空状态
      {
          USART_ReceiveData(USART3);
      }

      return rtn ;
}

uint16_t crc_res;
uint8_t  modbus_timeout_hook(void * p)
{
    uint8_t rtn=0;
    MODBUS_CONTRAL_INFO * cm_p;

    cm_p = p;

    if(cm_p->state == MB_SEND_END)//如果是在发送完毕 等待接受状态   此时是一个300us秒的定时器 此处应将只接受引脚
    {
       
         cm_p->state = MB_WAIT_REC;
         modbus_hal_set_receive();                               //管脚置接受状态
         change_timer(cm_p->tm_p,1000);                         //改变定时器时间为1秒 此处为发送询问后等待回复的定时器

         //改变控制器状态
         //此处不用开启定时器
         //因为不知道什么时候下面会回复
    }
   else if(cm_p->state == MB_WAIT_REC)//如果是在发送完毕 等待接受状态   此时是一个1秒的定时器 超时是认为设备损坏
    {
       
           //此处是表示发送询问后没有返回    
           //此时将对应的帧删除，并重置控制结构体
           //并进行下一帧的发送
           cm_p->mf_p->rec_stru_p->state = REC_ERR0R;
           delet_mod_queue_head(get_modbus_queue_p());                      //删除队列的头
           cm_p->state = MB_IDLE;
           cm_p->tm_p  = 0;
           cm_p->mf_p  = 0;
           start_modbus_queue_send(get_modbus_queue_p(),cm_p);
          


    }
    else if(cm_p->state == MB_REC)   //如果再接受状态下定时器超时说明接受的数据完成  可将此帧删除
    {
        
         cm_p->state = MB_FRM_ITV;

         
         if(rec_stru_crc(cm_p->mf_p->rec_stru_p)==0)
         {
            cm_p->mf_p->rec_stru_p->state = REC_SUCCESS;
            //clear_data(get_sample_p());
         }
         else
         {
             cm_p->mf_p->rec_stru_p->state = REC_ERR0R;
         }

        // crc_res = CRC16(cm_p->mf_p->rec_p ,53);
         delet_mod_queue_head(get_modbus_queue_p());                      //删除队列的头

        
                                                                 
         change_timer(cm_p->tm_p,1000);                                      //改变定时器间隔 并刷新  3.5T的定时器  为了发送下一帧创造间隔 
                                                                             //因为帧之间的间隔为3.5T  因为前面已经试3T的超时了，所以此时在等待3T
                                                                             //此处为上一次接受帧和下一次询问帧之间的定时器
   

    }
    else  if(cm_p->state == MB_FRM_ITV)   //此时是帧间隔超时 说明上一帧的帧间隔已经过完 ，此时可发送下一帧
    {


               
          cm_p->state = MB_IDLE;
          cm_p->tm_p  = 0;
          cm_p->mf_p  = 0;
          start_modbus_queue_send(get_modbus_queue_p(),cm_p);
      
    }



    return rtn;
}




//定时询问的时间超时函数钩子
//建立定时器并且每过一段时间就向队列中压入询问的数据
//p 为需要压入数据的队列指针
//uint8_t  modbus_inquire_ped_hook(void * p)
//{
//   
//    uint16_t crc_result;
//
//
//    
//
//    MODBUS_FRAME_INOF  modbus_stru={ {      30,     0x03,    0x00,              0x00,              0x00,            0x19,             0x00,             0x00}  ,
//                                     /*设备地址   功能     数据起始地址(高)   数据起始地址(低)   数据长度(高)     数据长度(低)      CRC16验校(高)     CRC验校(低)*/
//                                     8,
//                                     0,
//                                     0,
//                                  
//                                    };
//
//
//
//
//     crc_result =  CRC16( modbus_stru.send_str, 6);    //计算CRC；
//     modbus_stru.send_str[6] = crc_result>>8;
//     modbus_stru.send_str[7] = crc_result; 
//     
//    creat_timer((void *)get_modbus_queue_p(),10000,modbus_inquire_ped_hook);
//    insert_queue_tail((MODBUS_QUEUE_INFO*) p, &modbus_stru );
//     // insert_queue_tail((MODBUS_QUEUE_INFO*) p, &modbus_stru );
//     //insert_queue_tail((MODBUS_QUEUE_INFO*) p, &modbus_stru );
//     return   start_modbus_queue_send(get_modbus_queue_p(),modbus_get_cm_p());
//}





 /* CRC 高位字节值表 */ 
static unsigned char auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
} ; 
/* CRC低位字节值表*/ 
static char auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
} ;


/*CRC16 算法函数*/
unsigned short CRC16(unsigned char * puchMsg,unsigned short usDataLen) 
//puchMsg ; /* 要进行CRC校验的消息 */ 
//usDataLen ; /* 消息中字节数 */ 
{ 
  unsigned char uchCRCHi = 0xFF ; /* 高CRC字节初始化 */ 
  unsigned char uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */ 
  unsigned uIndex ; /* CRC循环中的索引 */ 


  while (usDataLen--) /* 传输消息缓冲区 */ 
  { 
    uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */ 
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
    uchCRCLo = auchCRCLo[uIndex] ; 
  } 

  return (uchCRCHi << 8 | uchCRCLo) ; 
} 
