#include "in_can.h"
#include "systick.h"
#include "temp_sample.h"
#include "init_gpio.h"
#include "hit.h"

//定义canbus的队列
CANBUS_QUEUE_INFO canbus_queue;



//得到队列的指针
CANBUS_QUEUE_INFO * get_canbus_queue_p(void)
{
    return &canbus_queue;
}

//初始化队列结构体
uint8_t  init_canbus_queue(CANBUS_QUEUE_INFO* queue_p)
{
   uint8_t rtn=0 ;


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

uint8_t  insert_can_queue_tail(CANBUS_QUEUE_INFO* queue_p, CanTxMsg * cantx_p )
{
      uint8_t  rtn = 0;
      uint8_t  i;

      if((queue_p->tail - queue_p->head + CAN_QUEUE_NUM)%CAN_QUEUE_NUM >= (CAN_QUEUE_NUM-2))  //当队列已满
      {
         rtn = 1;                                            //返回队列溢出
      }
      else
      {

         //将发送部分的数据复制
          queue_p->can_tx_str[queue_p->tail].StdId  = cantx_p->StdId;

          queue_p->can_tx_str[queue_p->tail].ExtId  = cantx_p->ExtId ;

          queue_p->can_tx_str[queue_p->tail].IDE    = cantx_p->IDE;

          queue_p->can_tx_str[queue_p->tail].RTR    = cantx_p->RTR;

          queue_p->can_tx_str[queue_p->tail].DLC    = cantx_p->DLC;

          if(cantx_p->RTR == CAN_RTR_DATA)
          {
              for(i=0;i<cantx_p->DLC;i++)
              {
                  queue_p->can_tx_str[queue_p->tail].Data[i]   = cantx_p->Data[i];
              }
          }
          //queue_p->can_tx_str[queue_p->tail].FMI

           
         //队列加1
          queue_p->tail ++; 

          if(queue_p->tail == CAN_QUEUE_NUM )             //队尾是不是到了数组的尾部
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



//删除can队头的数据
uint8_t  delet_can_queue_head(CANBUS_QUEUE_INFO* queue_p)
{
  uint8_t  rtn =0;


  if(queue_p->head != queue_p->tail)    //当队头不等于对尾  即队列不为空
  {
       queue_p->head ++;                //将队头加一   即删去了队头

       if(queue_p->head >= CAN_QUEUE_NUM)     //如果队头已经指向了数组外面
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
CanTxMsg *  get_can_queue_head_p(CANBUS_QUEUE_INFO* queue_p)
{
    CanTxMsg * rtn_p;
    if(queue_p->tail == queue_p->head)
    {
      rtn_p =  0;
    }
    else
    {
       rtn_p = &(queue_p->can_tx_str[queue_p->head]);
    }


    return rtn_p ;
}





//开始发送队列头的第一个字节 并对控制结构体置值
uint8_t  start_canbus_queue_send(CANBUS_QUEUE_INFO* queue_p)
{
     uint8_t rtn=0;
         if((queue_p->head != queue_p->tail)&&(queue_p->state == CAN_QUEUE_IDLE))
          {
     
               queue_p->state = CAN_QUEUE_BUSY ;
               CAN_Transmit  ( CAN1,  queue_p->can_tx_str + queue_p->head); 



                
          }
          else
          {
              rtn =1;
               //mc_p->tm_p  = 0;            //无定时器
          }


          return rtn;
}

//can 的队列发送中断
uint8_t   can_queue_send_irq(CANBUS_QUEUE_INFO* queue_p)
{
     uint8_t rtn=0;

     delet_can_queue_head(queue_p);
     queue_p->state = CAN_QUEUE_IDLE ;
     if(queue_p->head != queue_p->tail)
     {
        start_canbus_queue_send(queue_p); 
     }
     else
     {
         rtn =( queue_p->tail - queue_p->head + CAN_QUEUE_NUM )%CAN_QUEUE_NUM;
     }

     return rtn;
}




/*****************************************一下部分是和打壳相关的******************************************************************/
//向内can队列中写入询问锤头设置帧


//询问锤头状态
uint8_t ican_ask_hit_state(uint8_t channel)
{
     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x500|channel ;
     tx_msg.RTR   = CAN_RTR_REMOTE;
     tx_msg.IDE   = CAN_ID_EXT;

     rtn =  insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}
//询问锤头反馈值
uint8_t ican_ask_hit_fdbk(void)
{
     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x520;
     tx_msg.RTR   = CAN_RTR_REMOTE;
     tx_msg.IDE   = CAN_ID_EXT;

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}
//询问锤头状态
uint8_t ican_ask_bath_volt(void)
{
     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x50f;
     tx_msg.RTR   = CAN_RTR_REMOTE;
     tx_msg.IDE   = CAN_ID_EXT;

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}


//询问打壳设置
uint8_t ican_ask_hit_set(uint8_t channel)
{
     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x550|channel ;
     tx_msg.RTR   = CAN_RTR_REMOTE;
     tx_msg.IDE   = CAN_ID_EXT;

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}

uint8_t  ican_ask_history(uint8_t chanel ,uint8_t pre_num)
{

     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x510|chanel ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 1;

     tx_msg.Data[0] = 0x30;

    
     tx_msg.Data[chanel-1]=pre_num;

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
   
}
//内can定期询问 序列  一直询问锤头状态和槽压 反馈值
uint8_t ican_ask_sequence_prd(void)
{
   uint8_t rtn = 0;
   uint8_t i;
  
   rtn += ican_ask_hit_fdbk();

   for(i=1;i<= HIT_NUM;i++)
   {
     rtn +=  ican_ask_hit_state(i);
   }
   rtn += ican_ask_bath_volt();
   return rtn ;
}
uint8_t  ican_set_hit_mode(uint8_t ch)
{


     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x560|ch ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 8;

     tx_msg.Data[0] = get_hit_p(ch-1)->hit_time;
     tx_msg.Data[1] = get_hit_p(ch-1)->hit_int_time;
     tx_msg.Data[2] = get_hit_p(ch-1)->hold_time;
     tx_msg.Data[3] = get_hit_p(ch-1)->history_num/10;
     tx_msg.Data[4] = get_hit_p(ch-1)->rehit_num;
     //tx_msg.Data[5] = 0x30;
    
  

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}
//某一锤头加打壳次数
uint8_t  ican_add_hit_num(uint8_t chanel)
{

     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x540 ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 6;

     tx_msg.Data[0] = 0x30;
     tx_msg.Data[1] = 0x30;
     tx_msg.Data[2] = 0x30;
     tx_msg.Data[3] = 0x30;
     tx_msg.Data[4] = 0x30;
     tx_msg.Data[5] = 0x30;
    
     tx_msg.Data[chanel-1]=0x31;

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}

uint8_t ican_set_hit_num(uint8_t ch ,uint8_t num)  
{


     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x540 ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 6;

     tx_msg.Data[0] = 0x30;
     tx_msg.Data[1] = 0x30;
     tx_msg.Data[2] = 0x30;
     tx_msg.Data[3] = 0x30;
     tx_msg.Data[4] = 0x30;
     tx_msg.Data[5] = 0x30;
    
     tx_msg.Data[ch-1]=num;

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}

//奇数点加打壳次数
uint8_t  ican_add_odd_hit_num(void)
{
     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x540 ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 6;

     tx_msg.Data[0] = 0x31;
     tx_msg.Data[1] = 0x30;
     tx_msg.Data[2] = 0x31;
     tx_msg.Data[3] = 0x30;
     tx_msg.Data[4] = 0x31;
     tx_msg.Data[5] = 0x30;
    
   

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}


//偶数点加打壳次数
uint8_t  ican_add_even_hit_num(void)
{
     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x540 ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 6;

     tx_msg.Data[0] = 0x30;
     tx_msg.Data[1] = 0x31;
     tx_msg.Data[2] = 0x30;
     tx_msg.Data[3] = 0x31;
     tx_msg.Data[4] = 0x30;
     tx_msg.Data[5] = 0x31;
    
   

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}



//某一锤头 设置打壳保持 或取消打壳保持  是对当前状态的取反
uint8_t  ican_reversal_hit_hold(uint8_t chanel)
{
     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x540 ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 6;

     tx_msg.Data[0] = 0x30;
     tx_msg.Data[1] = 0x30;
     tx_msg.Data[2] = 0x30;
     tx_msg.Data[3] = 0x30;
     tx_msg.Data[4] = 0x30;
     tx_msg.Data[5] = 0x30;
    
   
     tx_msg.Data[chanel-1]=0xff;
     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}

// 取反 奇数点的 锤头保持
uint8_t  ican_reversal_odd_hit_hold(void)
{


     CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x540 ;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 6;

     tx_msg.Data[0] = 0xff;
     tx_msg.Data[1] = 0x30;
     tx_msg.Data[2] = 0xff;
     tx_msg.Data[3] = 0x30;
     tx_msg.Data[4] = 0xff;
     tx_msg.Data[5] = 0x30;
    
     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}
//取反偶数点的打壳保持
uint8_t  ican_reversal_even_hit_hold(void)
{
      CanTxMsg  tx_msg;
     uint8_t rtn =0;

     tx_msg.ExtId = 0x540;
     tx_msg.RTR   = CAN_RTR_DATA;
     tx_msg.IDE   = CAN_ID_EXT;
     tx_msg.DLC   = 6;

     tx_msg.Data[0] = 0x30;
     tx_msg.Data[1] = 0xff;
     tx_msg.Data[2] = 0x30;
     tx_msg.Data[3] = 0xff;
     tx_msg.Data[4] = 0x30;
     tx_msg.Data[5] = 0xff;
    
   

     rtn = insert_can_queue_tail(get_canbus_queue_p(),&tx_msg);
     rtn += start_canbus_queue_send(get_canbus_queue_p());
     return rtn ;
}




extern uint16_t bath_volt;

uint8_t  ican_rec_analyse(CanRxMsg * RxMessage)
{
  uint8_t rtn = 0;
  uint8_t i;
  uint8_t ch;
    if((RxMessage->RTR==CAN_RTR_DATA)
      &&((RxMessage->ExtId&0xfffffff0)==0X00000500)
      &&(RxMessage->ExtId&0x0000000f)>0
      &&(RxMessage->ExtId&0x0000000f)<7)  //如果返回的是add<<8 |0x0x   为x号锤头状态值
   {

         ch = (RxMessage->ExtId&0x0000000f)-1;

         get_hit_p(ch)->state =  RxMessage->Data[0]&0x0f;
         get_hit_p(ch)->num   = (RxMessage->Data[0]&0xf0)>>4 ;

         if((get_hit_p(ch)->state==HIT_STATE_ACT)||(get_hit_p(ch)->state==HIT_STATE_HLD))
         {
            get_hit_p(ch)->opt=1;
         }
         else
         {
            get_hit_p(ch)->opt=0; 
         }

         get_hit_p(ch)->ad_value   =  RxMessage->Data[1]|(RxMessage->Data[2]<<8);
         get_hit_p(ch)->blk_pro    =  RxMessage->Data[3];
         get_hit_p(ch)->lock_pro   =  RxMessage->Data[4];
         get_hit_p(ch)->big_pro    =  RxMessage->Data[5];
         get_hit_p(ch)->lkg_pro    =  RxMessage->Data[6];
         get_hit_p(ch)->line_break =  RxMessage->Data[7];
  
   }
   else   if( (RxMessage->RTR   == CAN_RTR_DATA)
            &&(RxMessage->ExtId == 0x0000050F))
   {
          


            bath_volt =   RxMessage->Data[0]|(RxMessage->Data[1]<<8);
  

   }
   else   if((RxMessage->RTR==CAN_RTR_DATA)&&((RxMessage->ExtId&0xfffffff0)==0X00000510)
          &&((RxMessage->ExtId&0x0000000f)>0)
          &&((RxMessage->ExtId&0x0000000f)<7)
           &&(RxMessage->DLC==8))  
    //如果发送的远程扩展帧 帧ID==0X1x 为历史记录返回帧
   {
     
       ch =  (RxMessage->ExtId&0X0000000f)-1;

       get_hit_p(ch)->nor_ratio  =   RxMessage->Data[0];
       get_hit_p(ch)->high_ratio =   RxMessage->Data[1];
       get_hit_p(ch)->low_ratio  =   RxMessage->Data[2];

       get_hit_p(ch)->idle_volt =    RxMessage->Data[3]|(RxMessage->Data[4]<<8) ;
       get_hit_p(ch)->idle_dev =     RxMessage->Data[5];
      
  

   }
   else   if((RxMessage->RTR==CAN_RTR_DATA)&&(RxMessage->ExtId==0X00000520)) //如果收到5号板的打壳反馈输入信号 
   {
  
        for(i=0;i<HIT_NUM;i++)
        {
          
            if(RxMessage->Data[0] & (1<<i))
            {
               get_hit_p(i)->feed_back=1;

            }
            else
            {
               get_hit_p(i)->feed_back=0;
            }

        }
   }
   else if((RxMessage->RTR==CAN_RTR_DATA)&& ((RxMessage->ExtId&0xfffffff0)==0x550) &&(RxMessage->DLC==8)) //如果是收到了询问打壳设置的返回帧
   {
                 ch= (RxMessage->ExtId&0x0f)-1;

                 get_hit_p(ch)->hit_time     = RxMessage->Data[0];
                 get_hit_p(ch)->hit_int_time = RxMessage->Data[1] ;
                 get_hit_p(ch)->hold_time    = RxMessage->Data[2] ;
                 get_hit_p(ch)->history_num  = RxMessage->Data[3]*10 ;
                 get_hit_p(ch)->rehit_num    = RxMessage->Data[4];
                 
            
   }

  return rtn;
}


