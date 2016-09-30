#include "DM163.H"
#include "init_spi.h"
#include "systick.h"
extern const unsigned char font8_8[][8] ;
                              // R   g   b



//���������ʾ�Ľṹ��
MATRIX_DATA_INFO       matrix_data_str ;
//��������������͵� ����
MATRIX_SEND_MSG_QUEUE  matrix_send_queue;
//�������Ŀ��ƽṹ��
MATRIX_CONTRAL_INFO   matrix_crl;

/****************************************************
          ������Ӳ����ز���
          �˲�����ֱ�Ӳ����������Ĺܽ�
          ��ҪΪ��λ����ʾʹ�� ������Ȳ���

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

//PC678 Ϊ��ѡ��
//�������Ϊ0 - 7 
void light_matrix_row(uint8_t temp)
{
   if(temp>7)
   {
       return ;
   }
   else
   {
         GPIOC->BSRR |= temp<<6;       //���ö�Ӧ��λΪ1
         GPIOC->BRR  |=((~temp)&(0x0007))<<6;   //�����Ӧ��λΪ0
   }
}

void matrix_hal_init(void)
{
   uint32_t  i;
   pin_matrix_rst;            //rst��  Ϊ��ʱ��λ    Ϊ�ߵ�ʱ����
   for(i=0;i<0xfffff;i++);
   pin_matrix_run;            //rst��  Ϊ��ʱ��λ    Ϊ�ߵ�ʱ����

   pin_disable_matrix;
   pin_enable_matrix;         //en��   Ϊ�͵�ʱ��ʹ��  Ϊ�ߵ�ʱ���ֹ

   matrix_lat(0);             //�����һֱ����Ϊ��  ֻ������Ҫ�������ڶ�һ�� 

   pin_select_fg;            //Ĭ��ѡ��ǰ��ɫ ֻ����Ҫ���ͱ���ɫʱ��ѡ����ɫ 
 // SetGamma();

  
}



/********************************************************************************
     ���������ݽṹ����ز���
    ���ڵ�����������һ�����ݽṹ��

****************************************************************************/

//���ص���ṹ�������ͷָ��
MATRIX_DATA_INFO *get_matrix_data_p (void)
{

  return     &matrix_data_str ;
}

//��ʼ��������������
uint8_t init_matrix_data_str(MATRIX_DATA_INFO * p)
{
    uint8_t i ,j,k;

    for(i=0;i<MATRIX_NUM;i++)
    {

      //���õ�i��ģ�� ����������ɫ
       p->bk_color[i][0]= 60;
       p->bk_color[i][1]= 60;
       p->bk_color[i][2]= 60;

       //����ɫ��չ����Ӧ��������ȥ
       extra_bk_color(p->bk_color_str[i] ,  p->bk_color[i]);

      for(k=0;k<8;k++)
      {
          for(j=0;j<8;j++)
          {
              //���õ�i��ģ��� j �������ǰ��ɫ
              p->fg_color_str[k][i][j][0] = 0;
              p->fg_color_str[k][i][j][1] = 0;
              p->fg_color_str[k][i][j][2] = 0;
          }
      }
    }


    return 0;
}

//��һ�������ֽڵı���ɫ���� ��չΪ18���ֽڵ���ģ������
//ԭ��������6λռ��һ���ֽ�   Ŀ���ǽ�ÿ6��λ�еĶ�����λɾ�����������������������
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

//���ر��������ͷ��ַ
uint8_t * get_bsk_str_p(MATRIX_DATA_INFO * p)
{
       return (uint8_t *)p->bk_color_str ;
}
//����ǰ��ɫ��
uint8_t * get_fg_str_p(MATRIX_DATA_INFO * p)
{
      return (uint8_t *)p->fg_color_str;
}

/*********************************************************************

���������Ͷ��в���
�Ե�����ÿ��ˢ��һ�� 
��ˢ�µ�����д�����
�˲�������Զ��еĲ���
*********************************************************************/
//���ض��е�ָ��
MATRIX_SEND_MSG_QUEUE * get_matrix_queue_p(void)
{

   return   &matrix_send_queue;
}
//��ʼ������
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
 //��һ�����͵�����֡���뵽����βȥ
//queue_p  ����ָ��
//modbus_p ����������ָ��

uint8_t  insert_matrix_queue_tail(MATRIX_SEND_MSG_QUEUE* queue_p, MATRIX_SEND_MSG_INFO * msg_p )
{
      uint8_t  rtn = 0;
     

      if((queue_p->tail - queue_p->head + MATIRX_QUEUE_NUM)%MATIRX_QUEUE_NUM >= (MATIRX_QUEUE_NUM-2))  //����������
      {
         rtn = 1;                                            //���ض������
         /*********************************/
         spi2_dma_init();                              //��ʼ��spi��dma
         init_matrix_msg_queue(queue_p);
         get_matrix_crl_p()->state = MATRIX_CRL_IDLE;
      }
      else
      {

         //����������е����ݸ���
          queue_p->msg_str[queue_p->tail].type = msg_p->type; 

          queue_p->msg_str[queue_p->tail].num  = msg_p->num;

          queue_p->msg_str[queue_p->tail].str_p = msg_p->str_p;
           
         //����β��1
          queue_p->tail ++; 

          if(queue_p->tail == MATIRX_QUEUE_NUM )             //��β�ǲ��ǵ��������β��
          {
              queue_p->tail =  0;
          }


   

      }



      return rtn;
}


//ɾ����ͷ������
uint8_t  delet_matrix_queue_head(MATRIX_SEND_MSG_QUEUE * queue_p)
{
  uint8_t  rtn =0;


  if(queue_p->head != queue_p->tail)    //����ͷ�����ڶ�β  �����в�Ϊ��
  {
       queue_p->head ++;                //����ͷ��һ   ��ɾȥ�˶�ͷ

       if(queue_p->head >= MATIRX_QUEUE_NUM)     //�����ͷ�Ѿ�ָ������������
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
 ���������Ʋ��ֽṹ��
*/

//���ص��������ƽṹ��ָ��
MATRIX_CONTRAL_INFO *get_matrix_crl_p(void)
{
    return &matrix_crl;
}

uint8_t start_matrix_row_send(MATRIX_CONTRAL_INFO * crl_p)
{
    uint8_t rtn=0;

    if(crl_p->state == MATRIX_CRL_IDLE)
    {
         crl_p->msg_p =   get_matrix_queue_head_p(get_matrix_queue_p()); //���ض��е�ͷָ��

         if(crl_p->msg_p != 0)      //ͷָ������Ϊ0 ��ʼ����
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
         else          //ͷָ����Ϊ0 �������û������ ����1
         {
             rtn  =1;
         }
    }
    else             //������������æ
    {
        rtn =2;
    }



    return rtn ;
}
//�����жϺ���
uint8_t end_matrix_row_send_irp(MATRIX_CONTRAL_INFO * crl_p)
{
      uint8_t rtn=0 ;

      if(crl_p->msg_p->type == BK_TYPE)     //�����͵��Ǳ���ɫ ������״̬Ϊǰ��ɫ  ��
      {
          pin_select_fg;                    //����һ�㶼����ǰ��ɫ״̬�� ֻ�з��ͱ���ɫʱ�Ÿ�Ϊ����ɫ״̬
      }


      pin_lat_action;                       //��������


      light_matrix_row(get_matrix_data_p()->row_now);     //����ʾ�л����ղŷ��͵���

      delet_matrix_queue_head(get_matrix_queue_p());   //ɾ�����е�ͷָ��

      crl_p->state = MATRIX_CRL_IDLE;

      crl_p->msg_p =   get_matrix_queue_head_p(get_matrix_queue_p()); //���ض��е�ͷָ��

      

         if(crl_p->msg_p != 0)      //ͷָ������Ϊ0 ˵�������л������� ��ʼ����
         {
             if(crl_p->msg_p->type ==  BK_TYPE)
             {
               pin_select_bk;
             }
             crl_p->state = MATRIX_CRL_BUSY;

             

             start_dma_send(crl_p->msg_p->str_p ,crl_p->msg_p->num);
              ///////////////////////////
               
               
         }
         else          //ͷָ����Ϊ0 �������û������ ����1
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
//������в���ˢ�±���ɫ������
uint8_t  insert_queue_bkg(uint32_t color)
{
    uint8_t rtn =0;
    MATRIX_SEND_MSG_INFO  msg;
    uint8_t i;
       for(i=0;i<MATRIX_NUM;i++)
       {
          //���õ�i��ģ�� ����������ɫ
          get_matrix_data_p()->bk_color[i][0]= color;
          get_matrix_data_p()->bk_color[i][1]= color>>8;
          get_matrix_data_p()->bk_color[i][2]= color>>16;

           //����ɫ��չ����Ӧ��������ȥ
           extra_bk_color(get_matrix_data_p()->bk_color_str[i] ,  get_matrix_data_p()->bk_color[i]);
      }


    msg.type  = BK_TYPE;
    msg.num   = BK_NUM;
    msg.str_p = (uint8_t *)get_matrix_data_p()->bk_color_str;
    rtn       = insert_matrix_queue_tail(get_matrix_queue_p(), &msg ) ;
    rtn      += start_matrix_row_send(get_matrix_crl_p());
   return   rtn;
}
//  ������в���ˢ��ǰ�� ������  
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

//����ˢ��ǰ��ɫһ�� �Ĺ��Ӻ���  
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










