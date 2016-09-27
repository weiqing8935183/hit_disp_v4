#include "disp.h"

#include "stdio.h"
#include "systick.h"
extern const unsigned char font8_8[][8] ;

//��ͼ�ϻ�һ����
uint8_t  disp_point(fg des_p,uint8_t row,uint8_t list,uint32_t color )
{
    uint8_t matrix_num,matrix_list;
    uint8_t rtn =0;
    if(row>8||(list>32)||(color>0xff000000))
    {
       rtn =1;
    }
    else
    {
       matrix_num =  list/8;
       matrix_list = list%8;
       des_p[row][matrix_num][matrix_list][0] = color;
       des_p[row][matrix_num][matrix_list][1] = color>>8;
       des_p[row][matrix_num][matrix_list][2] = color>>16;
    }

    return rtn;
}

//�ڵ���������ʾһ���ַ�
uint8_t  disp_char(fg des_p,uint8_t num,uint32_t color ,uint8_t ch )
{
   uint8_t rtn=0;
   uint8_t red,gre,ble;
   uint8_t i,j;
   red = color;
   gre = color>>8;
   ble = color>>16;

   if((num<MATRIX_NUM)&&(color <0xff000000)&&(ch<131))
   {
       for(i=0;i<8;i++)
       {
          for(j=0;j<8;j++)
          {
               if(((*(*(font8_8+ch)+i)>>j)&0x01 )!= 0)
               {
                des_p[i][num][j][0]= red ;
                des_p[i][num][j][1]= gre;
                des_p[i][num][j][2]= ble;
               }
               else
               {
                des_p[i][num][j][0]=0;
                des_p[i][num][j][1]=0;
                des_p[i][num][j][2]=0; 
               }


           }
        }
   }
   else
   {
       rtn =1;
   }
   return rtn;
}

//�����ݻ���Ľṹ����д���ַ� ��ķ�ʽ  ��ԭ�е����ݲ��Ķ� ���¼ӵ����ݲ��û�ķ�ʽ��ʾ
//num �ڼ���������  0-3
//color  ��ɫ
//ch  �ַ� 
uint8_t   disp_or_char(fg des_p,uint8_t num,uint32_t color ,uint8_t ch)
{
       uint8_t red,gre,ble;
       uint8_t rtn =0 ;
       uint8_t i,j;
       red = color;
       gre = color>>8;
       ble = color>>16;
       if((num<MATRIX_NUM)&&(color <0xff000000)&&(ch<131))
       {

           for(i=0;i<8;i++)
           {
              for(j=0;j<8;j++)
              {
                   if(((*(*(font8_8+ch)+i)>>j)&0x01 )!= 0)
                   {
                       des_p[i][num][j][0] |= red ;
                       des_p[i][num][j][1] |= gre;
                       des_p[i][num][j][2] |= ble;
                   }
               }
            }
        }
        else
        {
            rtn =1;
        }


        return rtn;
}

//�ڵ���������ʾһ���ַ��� 
//��ʾ���ַ�����ǰ4���ַ�
uint8_t  disp_str(fg des_p,uint32_t color ,uint8_t * str )
{
   uint8_t rtn=0;
   uint8_t i;
   if((color <0xff000000))
   {
       for(i=0;i<MATRIX_NUM;i++)
       {
         if(*(str+i)<131)
         {
            rtn += disp_char(des_p,i,color ,*(str+i));
         }
       }
   }
   else
   {
       rtn =1;
   }
   return rtn;
}

//�ڵ���������ʾһ������
//��Ϊ����ʾ4λ ����ֻ����ʾ-999 �� 9999֮�����
//�����ڴ��� ��ʾover
uint8_t disp_int(fg p,uint32_t color ,int16_t num)
{
   uint8_t  ch_str[8];
   uint8_t rtn =0;

   if((num>9999)||(num<-999))
   {
       disp_str(p,RED ,"OVER") ;
       rtn = 1;
   }
  else
  {
     sprintf((char*)ch_str,"%4d",num);

     disp_str(p,color ,ch_str ) ;
   }


   return rtn;

}


//�ڵ���������ʾһ��������

uint8_t disp_float(fg p,uint32_t color ,float num)
{
   uint8_t  ch_str[8];
   uint8_t rtn =0;
   uint8_t point_at=0,i;

   if((num>=10000)||(num<=-1000))
   {
       disp_str(p,RED ,"OVER") ;
       rtn = 1;
   }
  else
  {
     sprintf((char*)ch_str,"%4f",num);

     for(i=0;i<4&&(*(ch_str+i)!='.');i++) ;


     if(i<=4)
     {
        point_at = i-1;
        for(;i<4;i++)
        {
           *(ch_str+i) = *(ch_str+i+1);

        }
     }

     
     if(point_at<4)
     {
         disp_str(p,color ,ch_str ) ;
         disp_point(p,6,point_at*8+6,color);
         
     }
    
   }


   return rtn;

}

//��������Ľṹ��Ķ�����д���ַ�ĳһ��   ע �˺���ֻд�������һ��
//num �ڼ���������  0-3
//ROW��Ҫ�������� ��������ָ���������  ��0 ��ʾ �����������һ��д�������������һ��  1��ʾ����������������ڶ���д���������������һ��
//color  ��ɫ
//ch  �ַ� 
uint8_t   disp_writr_data_top(fg des_p,uint8_t num,uint8_t row,uint32_t color ,uint8_t ch)
{
       uint8_t red,gre,ble;
       uint8_t j;
       uint8_t rtn =0 ;
       red = color;
       gre = color>>8;
       ble = color>>16;

       if((num<MATRIX_NUM)&&(color <0xff000000)&&(ch<163))
       {
          for(j=0;j<8;j++)            //j��ʾ ĳ������ģ�������
          {
               if(((*(*(font8_8+ch)+row)>>j)&0x01 )!= 0)
               {
                des_p[0][num][j][0]= red ;
                des_p[0][num][j][1]= gre;
                des_p[0][num][j][2]= ble;
               }
               else
               {
                des_p[0][num][j][0]=0;
                des_p[0][num][j][1]=0;
                des_p[0][num][j][2]=0; 
               }


           }
         }
         else
         {
            rtn =1 ;
         }


        return rtn ;;
}
//��������Ľṹ��ĵײ���д���ַ� ע�� �˺���ֻд�������һ��
//num �ڼ���������  0-3
//��Ҫ��������   ��������ָ���������  ��0 ��ʾ �����������һ��д�������������һ��
//color  ��ɫ
//ch  �ַ� 
uint8_t   disp_write_data_down(fg des_p,uint8_t num,uint8_t row,uint32_t color ,uint8_t ch)
{
       uint8_t red,gre,ble;
       uint8_t j;
       uint8_t rtn = 0;
       red = color;
       gre = color>>8;
       ble = color>>16;

       if((num<MATRIX_NUM)&&(color <0xff000000)&&(ch<163))
       {
          for(j=0;j<8;j++)            //j��ʾ ĳ������ģ�������
          {
               if(((*(*(font8_8+ch)+row)>>j)&0x01 )!= 0)
               {
                des_p[7][num][j][0]= red ;
                des_p[7][num][j][1]= gre;
                des_p[7][num][j][2]= ble;
               }
               else
               {
                des_p[7][num][j][0]=0;
                des_p[7][num][j][1]=0;
                des_p[7][num][j][2]=0; 
               }


           }
        }
        else
        {
            rtn =1;
        }


        return rtn;
}

//��������Ľṹ����������д���ַ� ע�� �˺���ֻд�������һ��
//num �ڼ���������  0-3
//��Ҫ��������   ��������ָ���������  ��0 ��ʾ �����������һ��д�������������һ��
//color  ��ɫ
//ch  �ַ� 
uint8_t   disp_write_data_left(fg des_p,uint8_t num,uint8_t row,uint32_t color ,uint8_t ch)
{
       uint8_t red,gre,ble;
       uint8_t j;
       uint8_t rtn=0;
       red = color;
       gre = color>>8;
       ble = color>>16;

       if((num<MATRIX_NUM)&&(color <0xff000000)&&(ch<163))
       {
          for(j=0;j<8;j++)            //j��ʾ ĳ������ģ�������
          {
               if((((*(*(font8_8+ch)+j)<<(7-row)))&(0x80) )!= 0)
               {
                des_p[j][num][0][0]= red ;
                des_p[j][num][0][1]= gre;
                des_p[j][num][0][2]= ble;
               }
               else
               {
                des_p[j][num][0][0]=0;
                des_p[j][num][0][1]=0;
                des_p[j][num][0][2]=0; 
               }


           }
        }
        else
        {
            rtn =1;
        }

     return rtn;
}
//��������Ľṹ����������д���ַ� ע�� �˺���ֻд�������һ��
//num �ڼ���������  0-3
//��Ҫ��������   ��������ָ���������  ��0 ��ʾ �����������һ��д�������������һ��
//color  ��ɫ
//ch  �ַ� 
uint8_t   disp_write_data_right(fg des_p,uint8_t num,uint8_t row,uint32_t color ,uint8_t ch)
{
       uint8_t red,gre,ble;
       uint8_t j;
       uint8_t rtn = 0;
       red = color;
       gre = color>>8;
       ble = color>>16;

       if((num<MATRIX_NUM)&&(color <0xff000000)&&(ch<163))
       {
          for(j=0;j<8;j++)            //j��ʾ ĳ������ģ�������
          {
               if(((*(*(font8_8+ch)+j))&(0x01<<row) )!= 0)
               {
                des_p[j][num][7][0]= red ;
                des_p[j][num][7][1]= gre;
                des_p[j][num][7][2]= ble;
               }
               else
               {
                des_p[j][num][7][0]=0;
                des_p[j][num][7][1]=0;
                des_p[j][num][7][2]=0; 
               }


           }
        }
        else
        {
            rtn =1 ;
        }

     return rtn;
}

//�������������ݳ�ĳһ�����ƶ�һ��
//dir ����       
 uint8_t  disp_move_data(fg fg_color_str, uint8_t  dir )
 {
     uint8_t rtn=0;
     uint8_t i,j,k;


     if(dir == DIR_D_U)
     {
          for(i=0;i<7;i++)        //��for�������� ��ÿһ�е�����������һ��     
          {                           //��Ϊֻ��8��  0-7  
                  for(j=0;j<MATRIX_NUM;j++)
                  {
                      for(k=0;k<8;k++)
                      {
                         fg_color_str[i][j][k][0]    = get_matrix_data_p()->fg_color_str[i+1][j][k][0] ;
                         fg_color_str[i][j][k][1]    = get_matrix_data_p()->fg_color_str[i+1][j][k][1] ;
                         fg_color_str[i][j][k][2]    = get_matrix_data_p()->fg_color_str[i+1][j][k][2] ;
                      }
                   }
          }
        
     }
     else if(dir == DIR_U_D)
     {
          for(i=6;i!=0xff;i--)        //��for�������� ��ÿһ�е�����������һ��     
          {                           //��Ϊֻ��8��  0-7  
              for(j=0;j<MATRIX_NUM;j++)
              {
                  for(k=0;k<8;k++)
                  {
                    fg_color_str[i+1][j][k][0]    = get_matrix_data_p()->fg_color_str[i][j][k][0] ;
                    fg_color_str[i+1][j][k][1]    = get_matrix_data_p()->fg_color_str[i][j][k][1] ;
                    fg_color_str[i+1][j][k][2]    = get_matrix_data_p()->fg_color_str[i][j][k][2] ;
                 }
              }
          }
     }
     else if(dir == DIR_L_R)
     {

         for(i=0;i<8;i++)
         {
             for(j = MATRIX_NUM-1;j!=0xff;j-- )
             {
                  for(k=6;k!=0XFF;k--)   //��i��  jģ��  ���������������ƶ�
                  {
                    fg_color_str[i][j][k+1][0]    = get_matrix_data_p()->fg_color_str[i][j][k][0] ;
                    fg_color_str[i][j][k+1][1]    = get_matrix_data_p()->fg_color_str[i][j][k][1] ;
                    fg_color_str[i][j][k+1][2]    = get_matrix_data_p()->fg_color_str[i][j][k][2] ;
                  }
                 if(j!=0)        //���j����0ģ��  ����һ��ģ������һ�� �Ƶ��¸�ģ��ĵ�һ��
                 {
                    fg_color_str[i][j][0][0]    = get_matrix_data_p()->fg_color_str[i][j-1][7][0] ;
                    fg_color_str[i][j][0][1]    = get_matrix_data_p()->fg_color_str[i][j-1][7][1] ;
                    fg_color_str[i][j][0][2]    = get_matrix_data_p()->fg_color_str[i][j-1][7][2] ;
                  }
              }
          }
     }
     else if(dir == DIR_R_L)
     {

         for(i=0;i<8;i++)
         {
             for(j = 0;j<MATRIX_NUM;j++ )
             {
                  for(k=0;k<8;k++)   //��i��  jģ��  ���������������ƶ�
                  {
                    fg_color_str[i][j][k][0]    = get_matrix_data_p()->fg_color_str[i][j][k+1][0] ;
                    fg_color_str[i][j][k][1]    = get_matrix_data_p()->fg_color_str[i][j][k+1][1] ;
                    fg_color_str[i][j][k][2]    = get_matrix_data_p()->fg_color_str[i][j][k+1][2] ;
                  }
                 if(j!=MATRIX_NUM-1)        //���j����0ģ��  ����һ��ģ������һ�� �Ƶ��¸�ģ��ĵ�һ��
                 {
                    fg_color_str[i][j][7][0]    = get_matrix_data_p()->fg_color_str[i][j+1][0][0] ;
                    fg_color_str[i][j][7][1]    = get_matrix_data_p()->fg_color_str[i][j+1][0][1] ;
                    fg_color_str[i][j][7][2]    = get_matrix_data_p()->fg_color_str[i][j+1][0][2] ;
                  }
              }
          }
     }


     return rtn;
 }

 //��������  ������  ��ָ���е�Դ���� ������Ŀ�� ��ȥ
uint8_t disp_copy_data_row(fg des,uint8_t des_row,fg src,uint8_t src_row)
{
    uint8_t rtn =0;
    uint8_t i,j;

    for(i=0;i<MATRIX_NUM;i++)
    {
        for(j=0;j<8;j++)
        {
           des[des_row][i][j][0]  =  src[src_row][i][j][0];
           des[des_row][i][j][1]  =  src[src_row][i][j][1];
           des[des_row][i][j][2]  =  src[src_row][i][j][2];
        }

    }

    return rtn ;
}

 //��������  ������  ��ָ���е�Դ���� ������Ŀ����ȥ
uint8_t disp_copy_data_line(fg des,uint8_t des_line,fg src,uint8_t src_line)
{
    uint8_t rtn =0;
    uint8_t i,j;

    uint8_t src_matrix,src_sub_line;
    uint8_t des_matrix,des_sub_line;

     src_matrix   =  src_line/8;
     src_sub_line =  src_line%8;
     des_matrix   =  des_line/8;
     des_sub_line =  des_line%8;
    for(i=0;i<8;i++)
    {

           for(j=0;j<8;j++)
           {
           des[i][des_matrix][des_sub_line][0]  =  src[i][src_matrix][src_sub_line][0];
           des[i][des_matrix][des_sub_line][1]  =  src[i][src_matrix][src_sub_line][1];
           des[i][des_matrix][des_sub_line][2]  =  src[i][src_matrix][src_sub_line][2];
          }

      
    }
    return rtn ;
}
/*************************************��̬��ʾ����**************************************************/
//��̬��ʾ 

DISP_DYNAMIC_CNL dy_crl;          //���嶯̬��ʾ�Ŀ������ṹ��

//���ض�̬�ؽṹ���ָ��
DISP_DYNAMIC_CNL * get_dy_p(void)
{
   return &dy_crl; 
}

//��
uint8_t disp_dynamic_str(DISP_DYNAMIC_CNL *cnl_p)
{
     uint8_t rtn =0 ,i;



      if(get_dy_p()->state == DY_IDLE)
      {

               get_dy_p()->type = DY_CHAR;
               get_dy_p()->have_done      = 0;

               if((cnl_p->dir == DIR_U_D)||(cnl_p->dir == DIR_D_U))         //����������˶�
               {
                   if(cnl_p->str_num%4 == 0)                                         //����������ñ�4����
                   {
                      get_dy_p()->need_done      = (cnl_p->str_num/4)*8;             //��Ҫ�˶��Ĵ���Ϊ ��str_num/4)*8
                   } 
                   else                                                              //������ܱ�4����
                   {
                      get_dy_p()->need_done      = (cnl_p->str_num/4+1)*8;           //��Ҫ�˶��Ĳ���
                   }
               }
               else if((cnl_p->dir == DIR_L_R)||(cnl_p->dir == DIR_R_L))         //����������˶�
               {
                      get_dy_p()->need_done      = cnl_p->str_num*8;             //��Ҫ�˶��Ĵ���Ϊ ��str_num)*8
               }
               else
               {
                     get_dy_p()->need_done = 0;
               }

               get_dy_p()->color          = cnl_p->color;
               get_dy_p()->interval       = cnl_p->interval;
               get_dy_p()->dir            = cnl_p->dir;
               if(cnl_p->dir != DIR_L_R)
               {
                   for(i=0;i<cnl_p->str_num;i++)
                   {
                         *(get_dy_p()->str+i)  = *(cnl_p->str+i);
                   }
               }
               else
               {
                   for(i=0;i<cnl_p->str_num;i++)
                   {
                      *(get_dy_p()->str+cnl_p->str_num-1-i)  = *(cnl_p->str+i);
                    }
               }

                disp_move_data(get_matrix_data_p()->fg_color_str,get_dy_p()->dir);

                if(get_dy_p()->dir == DIR_U_D)
                {
                    for(i=0;i<MATRIX_NUM;i++)
                    {
                         //��Ϊ�����������˶� �������Ƚ�������ַ�����7�ϵ�����
                         disp_writr_data_top(get_matrix_data_p()->fg_color_str,i,7,get_dy_p()->color,*(get_dy_p()->str+i));   
                    }
                 }
                 else if(get_dy_p()->dir == DIR_D_U)
                 {
                       for(i=0;i<MATRIX_NUM;i++)
                       {
                            disp_write_data_down(get_matrix_data_p()->fg_color_str,i,0,get_dy_p()->color,*(get_dy_p()->str+i));
                       }
                 }
                 else if(get_dy_p()->dir == DIR_L_R)
                 {
                       disp_write_data_left(get_matrix_data_p()->fg_color_str,0,7,get_dy_p()->color,*(get_dy_p()->str));
                 }
                 else if(get_dy_p()->dir == DIR_R_L)
                 {
                       disp_write_data_right(get_matrix_data_p()->fg_color_str,MATRIX_NUM -1,0,get_dy_p()->color,*(get_dy_p()->str));
                 }

                get_dy_p()->tm_p = creat_timer((void *) get_dy_p(),cnl_p->interval, disp_dynamic_str_hook);

                if(get_dy_p()->tm_p != 0)
                {
                    get_dy_p()->state == DY_BUSY;
                }

      }
      else
      {
         get_dy_p()->interval = get_dy_p()->interval/2 ;
         if(get_dy_p()->interval == 0 )
         {
             get_dy_p()->interval = 1;
         }
         change_timer(get_dy_p()->tm_p,get_dy_p()->interval);

         rtn =1;
      }
     return rtn ;
}

//��̬��ʾ���Ӻ��� 

uint8_t disp_dynamic_str_hook(void * p)
{
       uint8_t rtn=0;
       uint8_t i;

       DISP_DYNAMIC_CNL * dy_p;
    
       dy_p = p;


       dy_p->have_done ++ ;
       if(dy_p->have_done < dy_p->need_done)
       {
          //���ݳ�ָ���ķ����ƶ�һ��
          disp_move_data(get_matrix_data_p()->fg_color_str,dy_p->dir);

          if(dy_p->dir == DIR_U_D)
          {
              for(i=0;i<MATRIX_NUM;i++)     //��for�����������һ��д������
              {
                  disp_writr_data_top(get_matrix_data_p()->fg_color_str,
                                      i,
                                      7-(get_dy_p()->have_done%8),
                                      get_dy_p()->color,
                                     *(get_dy_p()->str+(dy_p->have_done/8)*4 +i));   //��Ϊ�����������˶� �������Ƚ�������ַ�����7�ϵ�����
              }
          }
          else if(dy_p->dir == DIR_D_U)
          {
               for(i=0;i<MATRIX_NUM;i++)     //��for����������ÿ��ģ�����һ��д������
              {
                  disp_write_data_down(get_matrix_data_p()->fg_color_str,i,(get_dy_p()->have_done%8),get_dy_p()->color,*(get_dy_p()->str+(dy_p->have_done/8)*4 +i));   //��Ϊ�����������˶� �������Ƚ�������ַ�����7�ϵ�����
              }
          }
          else if(dy_p->dir == DIR_L_R)
          {
              //������ߵ�һ��д������
              disp_write_data_left(get_matrix_data_p()->fg_color_str,0,7-(get_dy_p()->have_done%8),get_dy_p()->color,*(get_dy_p()->str+dy_p->have_done/8));
          }
          else if(dy_p->dir == DIR_R_L)
          {
              //�����ұߵ�һ��д������
             disp_write_data_right(get_matrix_data_p()->fg_color_str,MATRIX_NUM-1,(get_dy_p()->have_done%8),get_dy_p()->color,*(get_dy_p()->str+(dy_p->have_done/8)));
          }

          get_dy_p()->tm_p=creat_timer((void *) get_dy_p(),get_dy_p()->interval , disp_dynamic_str_hook);    //������򦶨ʱ��

          if(get_dy_p()->tm_p == 0)
          {
              get_dy_p()->state = DY_IDLE;
          }
       }
       else
       {
           dy_p->state = DY_IDLE;
       }


    return rtn ;
    
     
}



uint8_t disp_dynamic_data(void)
{
           uint8_t rtn=0;

           if((get_dy_p()->type == DY_DATA)&&(get_dy_p()->state == DY_IDLE))
           {
                get_dy_p()->have_done      = 0;
                if((get_dy_p()->dir == DIR_U_D)||(get_dy_p()->dir == DIR_D_U))
                {
                   get_dy_p()->need_done      = 8;
                }
                else if((get_dy_p()->dir == DIR_L_R)||(get_dy_p()->dir == DIR_R_L))
                {
                     get_dy_p()->need_done      = 32;
                }

                disp_move_data(get_matrix_data_p()->fg_color_str,get_dy_p()->dir);

                if(get_dy_p()->dir == DIR_U_D)
                {
                      disp_copy_data_row(get_matrix_data_p()->fg_color_str,0,get_dy_p()->data,7);
                 }
                 else if(get_dy_p()->dir == DIR_D_U)
                 {
                      disp_copy_data_row(get_matrix_data_p()->fg_color_str,7,get_dy_p()->data,0);
                 }
                 else if(get_dy_p()->dir == DIR_L_R)
                 {
                       disp_copy_data_line(get_matrix_data_p()->fg_color_str,0,get_dy_p()->data,31);
                 }
                 else if(get_dy_p()->dir == DIR_R_L)
                 {
                      disp_copy_data_line(get_matrix_data_p()->fg_color_str,31,get_dy_p()->data,0);  
                 }    

                 get_dy_p()->tm_p = creat_timer((void *) get_dy_p(),get_dy_p()->interval, disp_dynamic_data_hook);
                 if(get_dy_p()->tm_p!= 0)
                 {
                   get_dy_p()->state = DY_BUSY;
                 }
           }
           else if((get_dy_p()->type == DY_DATA)&&(get_dy_p()->state == DY_BUSY))
           {
              get_dy_p()->interval = get_dy_p()->interval/2 ;
              if(get_dy_p()->interval == 0 )
              {
                  get_dy_p()->interval = 1;
              }
              change_timer(get_dy_p()->tm_p,get_dy_p()->interval);

              rtn =1;
           }

      return rtn;
}

uint8_t disp_dynamic_data_hook(void * p)
{
       uint8_t rtn=0;

       DISP_DYNAMIC_CNL * dy_p;
    
       dy_p = p;

       dy_p->have_done ++ ;
       if(dy_p->have_done < dy_p->need_done)
       {

                disp_move_data(get_matrix_data_p()->fg_color_str,get_dy_p()->dir);

                if(get_dy_p()->dir == DIR_U_D)
                {
                      disp_copy_data_row(get_matrix_data_p()->fg_color_str,0,get_dy_p()->data,7-dy_p->have_done );
                 }
                 else if(get_dy_p()->dir == DIR_D_U)
                 {
                      disp_copy_data_row(get_matrix_data_p()->fg_color_str,7,get_dy_p()->data,dy_p->have_done );
                 }
                 else if(get_dy_p()->dir == DIR_L_R)
                 {
                       disp_copy_data_line(get_matrix_data_p()->fg_color_str,0,get_dy_p()->data,32-dy_p->have_done );
                 }
                 else if(get_dy_p()->dir == DIR_R_L)
                 {
                      disp_copy_data_line(get_matrix_data_p()->fg_color_str,31,get_dy_p()->data,dy_p->have_done );  
                 }    

                 get_dy_p()->tm_p = creat_timer((void *) get_dy_p(),get_dy_p()->interval, disp_dynamic_data_hook);
                 if(get_dy_p()->tm_p == 0)
                 {
                     get_dy_p()->state =DY_IDLE;
                 }

       }
       else
       {
         get_dy_p()->state = DY_IDLE;
         rtn =1;
       }
       return rtn;
}
/*************************�˲�������ʾ�����ܲ���**************************************************************/

DISP_ITEM_INFO disp_item;

uint8_t  init_disp_item(DISP_ITEM_INFO *p)
{ 
   uint8_t  rtn=0;

   if(p!=0)
   {
       p->m_item =8;
       p->s_item =8;
   }
   else
   {
       rtn =1;
   }

   return rtn ;
}
DISP_ITEM_INFO * disp_get_item_p(void)
{
    return    &disp_item;
}


uint8_t disp_led(uint8_t disp_num)
{
    GPIO_Write  (GPIOD,  disp_num ) ;

    return 0;
}


uint8_t disp_m_item(void)
{
   
   disp_led(1<<(disp_item.m_item-1)) ;

   return 0;
}


