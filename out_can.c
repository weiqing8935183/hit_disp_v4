
#include "out_can.h"
#include "init_gpio.h"
#include "init_mcp2515.h"
#include "hit.h"
#include "in_can.h"
#include "time.h"
#include "rtc.h"

void  out_can_rec_analyse(CanRxMsg *RxMessage)
{
    
  
    CanTxMsg TMsg;                       //�����͵ı���
    uint8_t add;                         //��ʱ�����ַ
    uint8_t ch;                          //ָ���Ǹ���ͷ
    int16_t volttemp;   
    struct tm time;



    add=get_addr();

      if ( ( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0x1000)) )     //��ͷ״̬ѯ��֡
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
   else if( ((RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0x1010)))     //��ͷ����ѯ��֡
          && (RxMessage->IDE==CAN_ID_EXT) 
          && (RxMessage->RTR==CAN_RTR_REMOTE) )
   {
          ch=RxMessage->ExtId&0x0000000f;
          TMsg.ExtId  = (add<<16)|(0x1010)|(ch);
          TMsg.IDE    = CAN_ID_EXT;
          TMsg.RTR    = CAN_RTR_DATA;
          TMsg.DLC    = 8;
        
          TMsg.Data[0]= get_hit_p(ch)->rehit_num;                                 //��⵽�����ظ���ǵĴ���
          TMsg.Data[1]= get_hit_p(ch)->hit_time/10;              //���ʱ��
       
          TMsg.Data[2]= get_hit_p(ch)->hit_int_time/10;        //��Ǽ��ʱ��
          
          TMsg.Data[3]= get_hit_p(ch)->blk_pro_add;            //���ӵ�ѹ��
          TMsg.Data[4]= get_hit_p(ch)->blk_pro_add;             //���ӵ�ѹ��
          TMsg.Data[5]= 0x04;                                   //�жϷ���
          TMsg.Data[6]= 0;                                      //��
          TMsg.Data[7]= get_hit_p(ch)->hold_time;                                     //��Ǽ��ʱ��
          MPC2515_CAN_SEND(&TMsg);
     
   }
      else if(( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0xA000)))     //���ʱ������֡
            && (RxMessage->IDE==CAN_ID_EXT) 
            && (RxMessage->RTR==CAN_RTR_DATA) )
   {
               ch=RxMessage->ExtId&0x0000000f;  
               get_hit_p(ch)->rehit_num    = RxMessage->Data[0];        //�ظ���Ǵ���
               get_hit_p(ch)->hit_time     = RxMessage->Data[1]*10;     //���ʱ��
               get_hit_p(ch)->hit_int_time = RxMessage->Data[2]*10;     //��Ǽ��ʱ��
               get_hit_p(ch)->blk_pro_add=RxMessage->Data[3]|(RxMessage->Data[4]<<8);                           
                                   
               get_hit_p(ch)->hold_time=RxMessage->Data[7];         //��Ǳ���ʱ��

               ican_set_hit_mode(ch);
                
   }
   else if(( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0xA100)))     //�������֡
            && (RxMessage->IDE==CAN_ID_EXT) 
            && (RxMessage->RTR==CAN_RTR_DATA) )
   {
            ch=RxMessage->ExtId&0x0000000f;  

            if(RxMessage->Data[0]!=0)                           /*�����Ϊ0 �������ô�Ǵ���*/
            {

               ican_set_hit_num(ch ,RxMessage->Data[0]);  
      
            }
            else                                                 
            {
               if((RxMessage->Data[2]!=0)&&(get_hit_p(ch)->state!=HIT_STATE_HLD))                          //���ô�Ǳ���
               {
                  ican_reversal_hit_hold(ch);
               }
               else if((RxMessage->Data[2]==0)&&(get_hit_p(ch)->state==HIT_STATE_HLD))                    //ȡ����Ǳ���
               {
                   ican_reversal_hit_hold(ch);
               }

            }
                
   }
  else if(( (RxMessage->ExtId&0xfffffff0) == ((add<<16)|(0xB000)))     //����յ�����ʱ������
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

/*�ⲿCAN �Ķ�ʱ���ͳ���*/
void out_can_prd(void)
{

  CanTxMsg TMsg;  

  uint8_t i;

          TMsg.ExtId  = (get_addr()<<16)|(0x0000);
          TMsg.IDE    = CAN_ID_EXT;
          TMsg.RTR    = CAN_RTR_DATA;
          TMsg.DLC    = 6;
        
          //  bit0 ���� 
          //  bit1 ©��
          //  bit3 ��
          //  bit4  ��
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


