#include "temp_sample.h"
#include "modbus_host.h"
#include "can.h"
#include "init_gpio.h"

SAMPLE_INFO_STRUCT sample[DEVICE_NUM];


SAMPLE_INFO_STRUCT * get_sample_p(void)
{
   return sample;
}



//初始化一个接收结构体
uint8_t init_rec_stru(RECEIVE_INFO_STRUCT *rec_p)
{
     uint8_t rtn=0;

     rec_p->state = REC_INQUIRE;
     rec_p->rec_num = 0;

     return rtn;
}

uint8_t  init_sample_str(SAMPLE_INFO_STRUCT * sm_p)
{

   uint8_t rtn=0,i=0;
   uint8_t j;

   for(i=0;i<DEVICE_NUM;i++)
   {
      (sm_p+i)->addr = 1+i;

       for(j=1;j<=11;j++)    //因为只使用22个采样点 所以只初始化22个点的位置坐标   剩下的备用
       {
           (sm_p+i)->posi_str[j]    = j + i*22;               //最高位不为1  为a面
           (sm_p+i)->posi_str[j+11] = 0x80|j +i*22;           //最高位为1    为b面
       }

   }
  return rtn ;
}
//定时询问的时间超时函数钩子
//建立定时器并且每过一段时间就向队列中压入询问的数据
//p 为需要压入数据的队列指针
uint8_t  sample_inquire_prd_hook(void * p)
{
   
    uint16_t crc_result;
    uint8_t  i=0;
    SAMPLE_INFO_STRUCT *sm_p ;
    

    MODBUS_FRAME_INOF  modbus_stru={ {      1,     0x03,    0x00,              0x00,              0x00,            0x19,             0x00,             0x00}  ,
                                     /*设备地址   功能     数据起始地址(高)   数据起始地址(低)   数据长度(高)     数据长度(低)      CRC16验校(高)     CRC验校(低)*/
                                     8,      //需要发送的字节
                                     0,      //已经发送的字节
                                     0,      //接受地址的指针
                                    
                                    };

     sm_p = (SAMPLE_INFO_STRUCT *)p;
     for(i=0;i<DEVICE_NUM;i++)
     {
            modbus_stru.send_str[0]=(sm_p+i)->addr;
            crc_result =  CRC16( modbus_stru.send_str, 6);    //计算CRC；
            modbus_stru.send_str[6] = crc_result>>8;
            modbus_stru.send_str[7] = crc_result; 

            init_rec_stru(&((sm_p+i)->rec_stru));
            modbus_stru.rec_stru_p  = &((sm_p+i)->rec_stru);
            insert_mod_queue_tail(get_modbus_queue_p(), &modbus_stru );
     }

    
   
     
     creat_timer(sample,30000,sample_inquire_prd_hook);
    
     creat_timer(sample,10000,sample_divice_timer_hook);

     return   start_modbus_queue_send(get_modbus_queue_p(),modbus_get_cm_p());
}




//检测CRC数据 输入参数为接收的结构体 
//返回0 成功  返回1 失败
uint8_t  rec_stru_crc(RECEIVE_INFO_STRUCT * rec_p)
{
    uint16_t  crc_res;
    uint8_t rtn ;

     crc_res =    CRC16(rec_p->rec_str ,53);

     if(crc_res == (rec_p->rec_str[53]<<8|rec_p->rec_str[54]))
     {
            rtn =0;
     }
     else
     {
           rtn =1;
     }


     return rtn;
}


//整理接收的数据
//将接受到的字符串数据 整理为16位的无符号整形
uint8_t  clear_up_data(SAMPLE_INFO_STRUCT * sm_p)
{
     uint8_t rtn =0;

     uint8_t i ;

     if(sm_p->rec_stru.state == REC_SUCCESS)
     {
        sm_p->state = DEVICE_NOR; 

        sm_p->device_temp = sm_p->rec_stru.rec_str[3]<<8|(sm_p->rec_stru.rec_str[4]);

        for(i=5 ;i<(48+5); i=i+2)
        {
            sm_p->temp_str[(i-5)/2] = sm_p->rec_stru.rec_str[i]<<8| sm_p->rec_stru.rec_str[i+1];
        }


     }
     else
     {
         sm_p->state = DEVICE_ERR;
         rtn =1;
     }

     return rtn;
}

//定时器钩子函数 
//
//当压入询问modubus帧时 同时创建定时器 在一段时间之后 更新 smple结构体，将接受到的数据合并
uint8_t  sample_divice_timer_hook(void * sm_p)
{
    uint8_t rtn;
    uint8_t i;
    
//    SAMPLE_INFO_STRUCT *p;

//      p= (SAMPLE_INFO_STRUCT *)sm_p;

      for(i=0;i<DEVICE_NUM;i++)
      {
          rtn +=   clear_up_data ((SAMPLE_INFO_STRUCT *)sm_p +i);
      }

//      p= (SAMPLE_INFO_STRUCT *)sm_p+3;
//      for(i=0;i<24;i+=3)
//      {
//          tx.StdId   = (0x80|get_addr())<<3;
//          tx.IDE     =  CAN_ID_STD;
//          tx.RTR     =  CAN_RTR_DATA;
//          tx.DLC     =  8;
//          tx.Data[0] = i;
//          tx.Data[1] = 0x3f;
//          tx.Data[2] = p->temp_str[i];
//          tx.Data[3] = p->temp_str[i]>>8;
//          tx.Data[4] = p->temp_str[i+1];
//          tx.Data[5] = p->temp_str[i+1]>>8;
//          tx.Data[6] = p->temp_str[i+2];
//          tx.Data[7] = p->temp_str[i+2]>>8;
//
//          insert_can_queue_tail(get_canbus_queue_p(), &tx);
//          start_canbus_queue_send(get_canbus_queue_p());
//      }
      
     return rtn;
}

