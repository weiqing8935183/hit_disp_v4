#include "SysTick.h"

TIMER_STRUCT_INFO  timer_str[TIMER_NUM];
TIMER_LIST_INFO    free_list,work_list;
/*
 * ��������SysTick_Init
 * ����  ������ϵͳ�δ�ʱ�� SysTick
 * ����  ����
 * ���  ����
 * ����  ���ⲿ���� 
 */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// 
	if (SysTick_Config(SystemCoreClock / 1000))	//  100us�ж�һ��
	{ 
		/* Capture error */ 
		while (1);
	}
		// �رյδ�ʱ��  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;


    //ʹ�ܵδ�ʱ��
    //SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}


//��ʼ����ʱ��������
void init_timer_list(void)
{
     uint8_t i;

     for(i=0;i<TIMER_NUM;i++)
     {
        timer_str[i].timer_para   = 0;
        timer_str[i].dec_num     = 0;
        timer_str[i].over_fun    = 0;
        timer_str[i].next_p      = timer_str+i+1;
     }

     timer_str[TIMER_NUM-1].next_p=0;


      free_list.head_p   =   timer_str;
      free_list.list_num =   TIMER_NUM ;

      work_list.head_p = 0;
      work_list.list_num = 0;
}



//������ʱ��
//����ֵΪ�����Ķ�ʱ��ָ��
//������0 ��Ϊ����ʧ��

//flag Ϊ (*fun)(void * ) ����ڲ���
//interval Ϊ��ʱ�����жϴ���  ���������
//(*fun)(void * ) Ϊ��ʱ����õ�ָ��

TIMER_STRUCT_INFO * creat_timer(void * flag,uint32_t  interval ,uint8_t (*fun)(void * ))
{
   TIMER_STRUCT_INFO * rtn=0;

   TIMER_STRUCT_INFO * tm_p;

   if( free_list.list_num != 0)
   {
       //�ӿ����б���ɾ��һ���ṹ��
       tm_p =  free_list.head_p ;
       free_list.head_p = tm_p->next_p;
       free_list.list_num -- ;


       //��ʱ���Ľṹ�帳ֵ
       tm_p->timer_para  = flag ;
       tm_p->dec_num     = interval;
       tm_p->set_num     = interval;
       tm_p->over_fun    = fun;


       if(work_list.list_num !=0)    //����Ѿ��ж�ʱ��ģ��
       {
           tm_p->next_p = work_list.head_p;
           work_list.head_p   =  tm_p;
             
           work_list.list_num    ++;


       }
       else                            //���û�ж�ʱ��ģ����ִ��
       {
          
            work_list.head_p   =  tm_p;
            tm_p->next_p       =  0;
            work_list.list_num    ++;
                 //ʹ�ܵδ�ʱ��
            SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
       }

       rtn =  work_list.head_p;
   }
   else
   {
        rtn = 0 ;//���ʾ������ʱ��ʧ��
   }


   return rtn;
}

//ˢ��һ����ʱ��
uint8_t  refresh_timer(TIMER_STRUCT_INFO *tm_p)
{
    uint8_t rtn=0;
    
    



    if(tm_p != 0)
    {
        tm_p->dec_num = tm_p->set_num;
    }
    else
    {
        rtn =1 ;
    }


    return rtn;
}

//ˢ��һ����ʱ��
//�ı�һ����ʱ���Ĳ��� ������sˢ����
uint8_t  change_timer(TIMER_STRUCT_INFO *tm_p,uint32_t interval)
{
    uint8_t rtn=0;
    
    



    if(tm_p != 0)
    {
        tm_p->set_num =  interval;
        tm_p->dec_num =  interval;
    }
    else
    {
        rtn =1 ;
    }


    return rtn;
}
//����һ����ʱ��
uint8_t  destory_timer(TIMER_STRUCT_INFO *tm_p)
{

    uint8_t rtn=0;
    TIMER_STRUCT_INFO *pre_p=0;
    TIMER_STRUCT_INFO *now_p= work_list.head_p ;

    if((tm_p!=0)&&(now_p!=0))
    {
        for(;(now_p != tm_p)&&(now_p != 0);pre_p=now_p,now_p= now_p->next_p);

        if((pre_p == 0)&&(now_p == tm_p)) //˵���ǵ�һ��
        {
          //ɾ���ýڵ�
          work_list.head_p = tm_p->next_p;
          work_list.list_num    --;

          if(work_list.list_num == 0)
          {
             // �رյδ�ʱ��  
	         SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk; 
          }

          tm_p->next_p = free_list.head_p;
          free_list.head_p = tm_p;
          free_list.list_num ++;

         }
         else if((pre_p != 0)&&(now_p  == tm_p))
        {
           //ɾ���м�Ľڵ�
           pre_p->next_p = tm_p->next_p;
           work_list.list_num    --;

          tm_p->next_p = free_list.head_p;
          free_list.head_p = tm_p;
          free_list.list_num ++;
        }
  
    }
    return rtn;

}


//��ʱ����ʱ�ж�
uint8_t timer_prd_irq(void)
{
    

    TIMER_STRUCT_INFO *now_p ,*tem_p;

    for(now_p = work_list.head_p;  now_p!=0; )
    {
       now_p->dec_num --;
       tem_p = now_p->next_p ;
       if(now_p->dec_num == 0)
       {
          

           now_p->over_fun(now_p->timer_para);
           if(now_p->dec_num == 0)
           {
             destory_timer(now_p);
           }

         
       }
       now_p = tem_p;
    }


  return   work_list.list_num;
}


