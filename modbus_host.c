
#include "modbus_host.h"
#include "math.h"




//����modbus��֡����
MODBUS_QUEUE_INFO   modbus_queue;


//����MODBUS �������Ľṹ��
MODBUS_CONTRAL_INFO modbus_contral;
/*
     �õ�modbus�������ݽṹ��ָ��
*/
MODBUS_QUEUE_INFO * get_modbus_queue_p(void)
{
    return &modbus_queue;
}

//��ʼ�����нṹ��
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
//��һ�����͵�����֡���뵽������ȥ
//queue_p  ����ָ��
//modbus_p ����������ָ��

uint8_t  insert_mod_queue_tail(MODBUS_QUEUE_INFO* queue_p, MODBUS_FRAME_INOF * modbus_p )
{
      uint8_t  rtn = 0;
      uint8_t  i;

      if((queue_p->tail - queue_p->head + QUEUE_NUM)%QUEUE_NUM >= (QUEUE_NUM-2))  //����������
      {
         rtn = 1;                                            //���ض������
      }
      else
      {

         //�����Ͳ��ֵ����ݸ���
          queue_p->frame_str[queue_p->tail].need_send_num = modbus_p->need_send_num; 

          queue_p->frame_str[queue_p->tail].have_send_num=0;

          for(i=0;i<modbus_p->need_send_num;i++)
          {
              queue_p->frame_str[queue_p->tail].send_str[i] =modbus_p->send_str[i] ;
          }
         //�����ܲ��ֵ����ݸ���
           queue_p->frame_str[queue_p->tail].rec_stru_p = modbus_p->rec_stru_p;
           
         //���м�1
          queue_p->tail ++; 

          if(queue_p->tail == QUEUE_NUM )             //��β�ǲ��ǵ��������β��
          {
              queue_p->tail =  0;
          }


          //�����ƽṹ��Ϊ����
          //����ͷ֡
          //�˴����ٿ�ʼ���� ����ͨ���ֶ���Ͱ����start_modbus_queue_send ����
         //start_modbus_queue_send(queue_p,modbus_get_cm_p());


      }



      return rtn;
}


//ɾ����ͷ������
uint8_t  delet_mod_queue_head(MODBUS_QUEUE_INFO* queue_p)
{
  uint8_t  rtn =0;


  if(queue_p->head != queue_p->tail)    //����ͷ�����ڶ�β  �����в�Ϊ��
  {
       queue_p->head ++;                //����ͷ��һ   ��ɾȥ�˶�ͷ

       if(queue_p->head >= QUEUE_NUM)     //�����ͷ�Ѿ�ָ������������
       {
          queue_p->head = 0;              //����ͷָ������ͷ
       }
   
  }
  else
  {
      rtn =1;
  }


   return rtn;
}



//�õ�����ͷ ��ָ��
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
//�õ����ƽṹ���ָ��
MODBUS_CONTRAL_INFO * modbus_get_cm_p( void)
{
    return  &modbus_contral;
}



//��ʼ���Ͷ���ͷ�ĵ�һ���ֽ� ���Կ��ƽṹ����ֵ
uint8_t  start_modbus_queue_send(MODBUS_QUEUE_INFO* queue_p,MODBUS_CONTRAL_INFO *mc_p)
{
     uint8_t rtn=0;
         if((mc_p->state==MB_IDLE)&&(queue_p->head != queue_p->tail))
          {
              //�����е�ͷ�ø����ƽṹ��
              mc_p->mf_p  = &queue_p->frame_str[queue_p->head];
              
              mc_p->tm_p  = 0;            //�޶�ʱ��


              modbus_hal_set_send();                 //�÷��͹ܽ�
             
              //ע��˴��Ĳ���˳��
              //�ȶ�״̬�Ĵ��� ����д���ݼĴ����ſ����USART_IT_TC��־
              //Ȼ����ܿ��жϣ���Ȼ�Ļ��жϿ����� ��һֱ�����жϲ���ѭ��
              USART_GetITStatus( USART3, USART_IT_TC );
              USART_SendData(USART3, *(modbus_contral.mf_p->send_str+modbus_contral.mf_p->have_send_num));	 //��������
              mc_p->mf_p->have_send_num++;
               mc_p->state = MB_SEND;
              USART_ITConfig  (USART3,USART_IT_TC,ENABLE) ; //�򿪷����ж�
             
                
          }
          else
          {
              rtn =1;
               //mc_p->tm_p  = 0;            //�޶�ʱ��
          }


          return rtn;
}
//modbus send irq
//modbus ���ֽڷ��ͳɹ� �ж�

uint8_t  modbus_send_irq(MODBUS_CONTRAL_INFO * cm_p)
{
  uint8_t rtn;
  if(cm_p->state ==  MB_SEND)
  {
     

      if(cm_p->mf_p->have_send_num < cm_p->mf_p->need_send_num)  //�������û�з�����
      {
             USART_SendData(USART3, *(cm_p->mf_p->send_str+cm_p->mf_p->have_send_num));	 //��������
             cm_p->mf_p->have_send_num ++;
      }
      else                                                              //��������Ѿ��������
      {
             cm_p->state = MB_SEND_END;                              //�ı������״̬ ���������
             USART_ITConfig  (USART3,USART_IT_TC,DISABLE) ;          //�رշ����ж�

            
             cm_p->tm_p=creat_timer(modbus_get_cm_p(),5,modbus_timeout_hook); //����һ��700us 4T��Ķ�ʱ�� ���˶�ʱ���������Ǹı䷢��״̬
             //�˴�Ӧ�ô򿪶�ʱ��
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
          change_timer(cm_p->tm_p,5);                                 //����һ1.5T����Ķ�ʱ��  Ϊ�˼���ֽڼ���ܵĵļ�����ܴ���1.5T
                                                                      //ʵ��ʹ�õ�ʱ����3TԼ510us  ��Ϊ�������ݴ��͵�ʱ�������ɴﵽ2.5T
          cm_p->mf_p->rec_stru_p->state = REC_BUSY;
          cm_p->mf_p->rec_stru_p->rec_str[cm_p->mf_p->rec_stru_p->rec_num]   =   USART_ReceiveData(USART3);
          cm_p->mf_p->rec_stru_p->rec_num ++;
        

      }
      else                           //���û�н������״̬ ���һ�¼Ĵ��������״̬
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

    if(cm_p->state == MB_SEND_END)//������ڷ������ �ȴ�����״̬   ��ʱ��һ��300us��Ķ�ʱ�� �˴�Ӧ��ֻ��������
    {
       
         cm_p->state = MB_WAIT_REC;
         modbus_hal_set_receive();                               //�ܽ��ý���״̬
         change_timer(cm_p->tm_p,1000);                         //�ı䶨ʱ��ʱ��Ϊ1�� �˴�Ϊ����ѯ�ʺ�ȴ��ظ��Ķ�ʱ��

         //�ı������״̬
         //�˴����ÿ�����ʱ��
         //��Ϊ��֪��ʲôʱ�������ظ�
    }
   else if(cm_p->state == MB_WAIT_REC)//������ڷ������ �ȴ�����״̬   ��ʱ��һ��1��Ķ�ʱ�� ��ʱ����Ϊ�豸��
    {
       
           //�˴��Ǳ�ʾ����ѯ�ʺ�û�з���    
           //��ʱ����Ӧ��֡ɾ���������ÿ��ƽṹ��
           //��������һ֡�ķ���
           cm_p->mf_p->rec_stru_p->state = REC_ERR0R;
           delet_mod_queue_head(get_modbus_queue_p());                      //ɾ�����е�ͷ
           cm_p->state = MB_IDLE;
           cm_p->tm_p  = 0;
           cm_p->mf_p  = 0;
           start_modbus_queue_send(get_modbus_queue_p(),cm_p);
          


    }
    else if(cm_p->state == MB_REC)   //����ٽ���״̬�¶�ʱ����ʱ˵�����ܵ��������  �ɽ���֡ɾ��
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
         delet_mod_queue_head(get_modbus_queue_p());                      //ɾ�����е�ͷ

        
                                                                 
         change_timer(cm_p->tm_p,1000);                                      //�ı䶨ʱ����� ��ˢ��  3.5T�Ķ�ʱ��  Ϊ�˷�����һ֡������ 
                                                                             //��Ϊ֮֡��ļ��Ϊ3.5T  ��Ϊǰ���Ѿ���3T�ĳ�ʱ�ˣ����Դ�ʱ�ڵȴ�3T
                                                                             //�˴�Ϊ��һ�ν���֡����һ��ѯ��֮֡��Ķ�ʱ��
   

    }
    else  if(cm_p->state == MB_FRM_ITV)   //��ʱ��֡�����ʱ ˵����һ֡��֡����Ѿ����� ����ʱ�ɷ�����һ֡
    {


               
          cm_p->state = MB_IDLE;
          cm_p->tm_p  = 0;
          cm_p->mf_p  = 0;
          start_modbus_queue_send(get_modbus_queue_p(),cm_p);
      
    }



    return rtn;
}




//��ʱѯ�ʵ�ʱ�䳬ʱ��������
//������ʱ������ÿ��һ��ʱ����������ѹ��ѯ�ʵ�����
//p Ϊ��Ҫѹ�����ݵĶ���ָ��
//uint8_t  modbus_inquire_ped_hook(void * p)
//{
//   
//    uint16_t crc_result;
//
//
//    
//
//    MODBUS_FRAME_INOF  modbus_stru={ {      30,     0x03,    0x00,              0x00,              0x00,            0x19,             0x00,             0x00}  ,
//                                     /*�豸��ַ   ����     ������ʼ��ַ(��)   ������ʼ��ַ(��)   ���ݳ���(��)     ���ݳ���(��)      CRC16��У(��)     CRC��У(��)*/
//                                     8,
//                                     0,
//                                     0,
//                                  
//                                    };
//
//
//
//
//     crc_result =  CRC16( modbus_stru.send_str, 6);    //����CRC��
//     modbus_stru.send_str[6] = crc_result>>8;
//     modbus_stru.send_str[7] = crc_result; 
//     
//    creat_timer((void *)get_modbus_queue_p(),10000,modbus_inquire_ped_hook);
//    insert_queue_tail((MODBUS_QUEUE_INFO*) p, &modbus_stru );
//     // insert_queue_tail((MODBUS_QUEUE_INFO*) p, &modbus_stru );
//     //insert_queue_tail((MODBUS_QUEUE_INFO*) p, &modbus_stru );
//     return   start_modbus_queue_send(get_modbus_queue_p(),modbus_get_cm_p());
//}





 /* CRC ��λ�ֽ�ֵ�� */ 
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
/* CRC��λ�ֽ�ֵ��*/ 
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


/*CRC16 �㷨����*/
unsigned short CRC16(unsigned char * puchMsg,unsigned short usDataLen) 
//puchMsg ; /* Ҫ����CRCУ�����Ϣ */ 
//usDataLen ; /* ��Ϣ���ֽ��� */ 
{ 
  unsigned char uchCRCHi = 0xFF ; /* ��CRC�ֽڳ�ʼ�� */ 
  unsigned char uchCRCLo = 0xFF ; /* ��CRC �ֽڳ�ʼ�� */ 
  unsigned uIndex ; /* CRCѭ���е����� */ 


  while (usDataLen--) /* ������Ϣ������ */ 
  { 
    uIndex = uchCRCHi ^ *puchMsg++ ; /* ����CRC */ 
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
    uchCRCLo = auchCRCLo[uIndex] ; 
  } 

  return (uchCRCHi << 8 | uchCRCLo) ; 
} 
