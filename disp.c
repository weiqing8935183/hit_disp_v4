#include "disp.h"

#include "stdio.h"
#include "systick.h"
extern const unsigned char font8_8[][8] ;

//在图上画一个点
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

//在点阵屏上显示一个字符
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

//向数据缓存的结构体中写入字符 或的方式  即原有的数据不改动 ，新加的数据采用或的方式显示
//num 第几个点阵屏  0-3
//color  颜色
//ch  字符 
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

//在点阵屏上显示一个字符串 
//显示此字符串的前4个字符
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

//在点阵屏上显示一个整数
//因为能显示4位 所以只能显示-999 至 9999之间的数
//若大于此数 显示over
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


//在点阵屏上显示一个浮点数

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

//向点阵屏的结构体的顶部中写入字符某一行   注 此函数只写最上面的一行
//num 第几个点阵屏  0-3
//ROW需要填充的行数 此行数是指字体的行数  如0 表示 将字体的最上一行写入点阵屏的最上一行  1表示将字体从上往下数第二行写入点阵屏的最上面一行
//color  颜色
//ch  字符 
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
          for(j=0;j<8;j++)            //j表示 某个点阵模块的列数
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
//向点阵屏的结构体的底部中写入字符 注意 此函数只写最下面的一行
//num 第几个点阵屏  0-3
//需要填充的行数   此行数是指字体的行数  如0 表示 将字体的最上一行写入点阵屏的最下一行
//color  颜色
//ch  字符 
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
          for(j=0;j<8;j++)            //j表示 某个点阵模块的列数
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

//向点阵屏的结构体的最左边中写入字符 注意 此函数只写最左面的一行
//num 第几个点阵屏  0-3
//需要填充的行数   此行数是指字体的行数  如0 表示 将字体的最右一列写入点阵屏的最左一列
//color  颜色
//ch  字符 
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
          for(j=0;j<8;j++)            //j表示 某个点阵模块的列数
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
//向点阵屏的结构体的最左边中写入字符 注意 此函数只写最左面的一行
//num 第几个点阵屏  0-3
//需要填充的行数   此行数是指字体的行数  如0 表示 将字体的最右一列写入点阵屏的最左一列
//color  颜色
//ch  字符 
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
          for(j=0;j<8;j++)            //j表示 某个点阵模块的列数
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

//将点阵屏的数据朝某一方向移动一步
//dir 方向       
 uint8_t  disp_move_data(fg fg_color_str, uint8_t  dir )
 {
     uint8_t rtn=0;
     uint8_t i,j,k;


     if(dir == DIR_D_U)
     {
          for(i=0;i<7;i++)        //此for的作用是 将每一行的数据向上移一行     
          {                           //因为只有8行  0-7  
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
          for(i=6;i!=0xff;i--)        //此for的作用是 将每一行的数据向下移一行     
          {                           //因为只有8行  0-7  
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
                  for(k=6;k!=0XFF;k--)   //将i行  j模块  的数据由左向右移动
                  {
                    fg_color_str[i][j][k+1][0]    = get_matrix_data_p()->fg_color_str[i][j][k][0] ;
                    fg_color_str[i][j][k+1][1]    = get_matrix_data_p()->fg_color_str[i][j][k][1] ;
                    fg_color_str[i][j][k+1][2]    = get_matrix_data_p()->fg_color_str[i][j][k][2] ;
                  }
                 if(j!=0)        //如果j不是0模块  则将上一个模块的最后一列 移到下个模块的第一列
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
                  for(k=0;k<8;k++)   //将i行  j模块  的数据由右向左移动
                  {
                    fg_color_str[i][j][k][0]    = get_matrix_data_p()->fg_color_str[i][j][k+1][0] ;
                    fg_color_str[i][j][k][1]    = get_matrix_data_p()->fg_color_str[i][j][k+1][1] ;
                    fg_color_str[i][j][k][2]    = get_matrix_data_p()->fg_color_str[i][j][k+1][2] ;
                  }
                 if(j!=MATRIX_NUM-1)        //如果j不是0模块  则将上一个模块的最后一列 移到下个模块的第一列
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

 //拷贝数据  行数据  将指定行的源数据 拷贝至目标 行去
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

 //拷贝数据  列数据  将指定列的源数据 拷贝至目标列去
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
/*************************************动态显示部分**************************************************/
//动态显示 

DISP_DYNAMIC_CNL dy_crl;          //定义动态显示的控制器结构体

//返回动态控结构体的指针
DISP_DYNAMIC_CNL * get_dy_p(void)
{
   return &dy_crl; 
}

//向
uint8_t disp_dynamic_str(DISP_DYNAMIC_CNL *cnl_p)
{
     uint8_t rtn =0 ,i;



      if(get_dy_p()->state == DY_IDLE)
      {

               get_dy_p()->type = DY_CHAR;
               get_dy_p()->have_done      = 0;

               if((cnl_p->dir == DIR_U_D)||(cnl_p->dir == DIR_D_U))         //如果是上下运动
               {
                   if(cnl_p->str_num%4 == 0)                                         //如果字数正好被4整除
                   {
                      get_dy_p()->need_done      = (cnl_p->str_num/4)*8;             //需要运动的次数为 （str_num/4)*8
                   } 
                   else                                                              //如果不能被4整除
                   {
                      get_dy_p()->need_done      = (cnl_p->str_num/4+1)*8;           //需要运动的不数
                   }
               }
               else if((cnl_p->dir == DIR_L_R)||(cnl_p->dir == DIR_R_L))         //如果是上下运动
               {
                      get_dy_p()->need_done      = cnl_p->str_num*8;             //需要运动的次数为 （str_num)*8
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
                         //因为是由上往下运动 所以最先进入的是字符数组7上的数据
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

//动态显示钩子函数 

uint8_t disp_dynamic_str_hook(void * p)
{
       uint8_t rtn=0;
       uint8_t i;

       DISP_DYNAMIC_CNL * dy_p;
    
       dy_p = p;


       dy_p->have_done ++ ;
       if(dy_p->have_done < dy_p->need_done)
       {
          //数据朝指定的方向移动一下
          disp_move_data(get_matrix_data_p()->fg_color_str,dy_p->dir);

          if(dy_p->dir == DIR_U_D)
          {
              for(i=0;i<MATRIX_NUM;i++)     //此for的作用是在最顶一行写入数据
              {
                  disp_writr_data_top(get_matrix_data_p()->fg_color_str,
                                      i,
                                      7-(get_dy_p()->have_done%8),
                                      get_dy_p()->color,
                                     *(get_dy_p()->str+(dy_p->have_done/8)*4 +i));   //因为是由上往下运动 所以最先进入的是字符数组7上的数据
              }
          }
          else if(dy_p->dir == DIR_D_U)
          {
               for(i=0;i<MATRIX_NUM;i++)     //此for的作用是在每个模块最底一行写入数据
              {
                  disp_write_data_down(get_matrix_data_p()->fg_color_str,i,(get_dy_p()->have_done%8),get_dy_p()->color,*(get_dy_p()->str+(dy_p->have_done/8)*4 +i));   //因为是由上往下运动 所以最先进入的是字符数组7上的数据
              }
          }
          else if(dy_p->dir == DIR_L_R)
          {
              //向最左边的一列写入数据
              disp_write_data_left(get_matrix_data_p()->fg_color_str,0,7-(get_dy_p()->have_done%8),get_dy_p()->color,*(get_dy_p()->str+dy_p->have_done/8));
          }
          else if(dy_p->dir == DIR_R_L)
          {
              //向最右边的一列写入数据
             disp_write_data_right(get_matrix_data_p()->fg_color_str,MATRIX_NUM-1,(get_dy_p()->have_done%8),get_dy_p()->color,*(get_dy_p()->str+(dy_p->have_done/8)));
          }

          get_dy_p()->tm_p=creat_timer((void *) get_dy_p(),get_dy_p()->interval , disp_dynamic_str_hook);    //创建下颚定时器

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
/*************************此部分是显示二极管部分**************************************************************/

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


