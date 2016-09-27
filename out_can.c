
#include "out_can.h"
#include "init_gpio.h"
#include "init_mcp2515.h"
#include "hit.h"
#include "in_can.h"
#include "time.h"
#include "rtc.h"

void  out_can_rec_analyse(CanRxMsg *RxMessage)
{
    
  
    CanTxMsg TMsg;                       //欲发送的变量
    uint8_t add;                         //暂时保存地址
    uint8_t ch;                          //指定那个锤头
    int16_t volttemp;   
    struct tm time;



    add=get_addr();

      if ( ( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0x1000)) )     //锤头状态询问帧
       && (RxMessage->IDE==CAN_ID_EXT) 
       && (RxMessage->RTR==CAN_RTR_REMOTE))
    {
          ch=RxMessage->ExtId&0x0000000f;
          TMsg.ExtId  =  (add<<16)|(0x1000)|(ch);
          TMsg.IDE    =  CAN_ID_EXT;
          TMsg.RTR    =  CAN_RTR_DATA;
          TMsg.DLC    =  8;
          volttemp    =  2000;
          TMsg.Data[0]=  volttemp;
          TMsg.Data[1]=  volttemp>>8;
          volttemp    =  3100;
          TMsg.Data[2]=  volttemp;
          TMsg.Data[3]=  volttemp>>8;
          TMsg.Data[4]=  0;
          TMsg.Data[5]=  0;
          TMsg.Data[6]=  0;
          TMsg.Data[7]=  0;
          MPC2515_CAN_SEND(&TMsg);

   }
   else if( ((RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0x1010)))     //锤头设置询问帧
          && (RxMessage->IDE==CAN_ID_EXT) 
          && (RxMessage->RTR==CAN_RTR_REMOTE) )
   {
          ch=RxMessage->ExtId&0x0000000f;
          TMsg.ExtId  = (add<<16)|(0x1010)|(ch);
          TMsg.IDE    = CAN_ID_EXT;
          TMsg.RTR    = CAN_RTR_DATA;
          TMsg.DLC    = 8;
        
          TMsg.Data[0]= get_hit_p(ch)->rehit_num;                                 //检测到卡堵重复打壳的次数
          TMsg.Data[1]= get_hit_p(ch)->hit_time/10;              //打壳时间
       
          TMsg.Data[2]= get_hit_p(ch)->hit_int_time/10;        //打壳间隔时间
          
          TMsg.Data[3]= get_hit_p(ch)->blk_pro_add;            //附加电压低
          TMsg.Data[4]= get_hit_p(ch)->blk_pro_add;             //附加电压高
          TMsg.Data[5]= 0x04;                                   //判断方法
          TMsg.Data[6]= 0;                                      //无
          TMsg.Data[7]= get_hit_p(ch)->hold_time;                                     //打壳间隔时间
          MPC2515_CAN_SEND(&TMsg);
     
   }
      else if(( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0xA000)))     //打壳时间设置帧
            && (RxMessage->IDE==CAN_ID_EXT) 
            && (RxMessage->RTR==CAN_RTR_DATA) )
   {
               ch=RxMessage->ExtId&0x0000000f;  
               get_hit_p(ch)->rehit_num    = RxMessage->Data[0];        //重复打壳次数
               get_hit_p(ch)->hit_time     = RxMessage->Data[1]*10;     //打壳时间
               get_hit_p(ch)->hit_int_time = RxMessage->Data[2]*10;     //打壳间隔时间
               get_hit_p(ch)->blk_pro_add=RxMessage->Data[3]|(RxMessage->Data[4]<<8);                           
                                   
               get_hit_p(ch)->hold_time=RxMessage->Data[7];         //打壳保持时间

               ican_set_hit_mode(ch);
                
   }
   else if(( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0xA100)))     //打壳设置帧
            && (RxMessage->IDE==CAN_ID_EXT) 
            && (RxMessage->RTR==CAN_RTR_DATA) )
   {
            ch=RxMessage->ExtId&0x0000000f;  

            if(RxMessage->Data[0]!=0)                           /*如果不为0 则是设置打壳次数*/
            {

               ican_set_hit_num(ch ,RxMessage->Data[0]);  
      
            }
            else                                                 
            {
               if((RxMessage->Data[2]!=0)&&(get_hit_p(ch)->state!=HIT_STATE_HLD))                          //设置打壳保持
               {
                  ican_reversal_hit_hold(ch);
               }
               else if((RxMessage->Data[2]==0)&&(get_hit_p(ch)->state==HIT_STATE_HLD))                    //取消打壳保持
               {
                   ican_reversal_hit_hold(ch);
               }

            }
                
   }
  else if(( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0xB000)))     //如果收到的是时间设置
            && (RxMessage->IDE==CAN_ID_EXT) 
            && (RxMessage->RTR==CAN_RTR_DATA) )
   {
             time.tm_year= RxMessage->Data[0]+2000;
             time.tm_mon=  RxMessage->Data[1]-1;
             time.tm_mday= RxMessage->Data[2];
             time.tm_hour= RxMessage->Data[3];
             time.tm_min=  RxMessage->Data[4];
             time.tm_sec=  RxMessage->Data[5];


             Time_SetCalendar(&time);
   }


         
            
      



   RxMessage->FMI=0;
}

/*外部CAN 的定时发送程序*/
void out_can_prd(void)
{

  CanTxMsg TMsg;  

  uint8_t i;

          TMsg.ExtId  = (get_addr()<<16)|(0x0000);
          TMsg.IDE    = CAN_ID_EXT;
          TMsg.RTR    = CAN_RTR_DATA;
          TMsg.DLC    = 6;
        
          //  bit0 断线 
          //  bit1 漏电
          //  bit3 卡
          //  bit4  堵
          for(i=0;i<HIT_NUM;i++)
          {
             TMsg.Data[i] = 0x00;
             if((get_hit_p(i)->blk_pro+get_hit_p(i)->blk_pro_add)>=BLOCK_LIMIT)
             {
                 TMsg.Data[i] |= 0x08;
             }

              if((get_hit_p(i)->lkg_pro)>=LEAKAGE_LIMIT)
             {
                 TMsg.Data[i] |= 0x02;
             }


             if((get_hit_p(i)->lock_pro+get_hit_p(i)->lock_pro_add)>=LOCK_LIMIT)
             {
                  TMsg.Data[i] |= 0x04;
             }
        


           if((get_hit_p(i)->line_break)>=BREAK_LIMIT)
                {
                   TMsg.Data[i] |= 0x01;
                }

          }

           MPC2515_CAN_SEND( &TMsg);
     


}


