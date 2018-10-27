/*--------------------------------------*-
文件：SCH51.H
时间：2016年3月30日
作者：HWY
版本：V1.0
-*---------------------------------------*/

#ifndef _SCH51_H
#define _SCH51_H

#include <reg52.h>
#include "config.h"
#include "t0_uart_init.h"

typedef unsigned char INT8U;
typedef unsigned int  INT16U;
typedef unsigned long INT32U;

#ifndef NULL
#define NULL                          (void *)0
#endif

#ifndef TRUE
#define FALSE                         0							//假
#define TRUE                          (!FALSE)			//真
#endif

#define RETURN_NORMAL                 0							//返回正常
#define RETURN_ERROR                  1							//返回错误

#define INTERRUPT_EXTERNAL_0          0							//外部中断0
#define INTERRUPT_TIM_0_OF            1	            //OF == OverFlow(溢出)
#define INTERRUPT_EXTERNAL_1          2							//外部中断1
#define INTERRUPT_TIM_1_OF            3
#define INTERRUPT_UART_RX_TX          4							//串口中断
#define INTERRUPT_TIM_2_OF            5

#define ERROR_SCH_TOO_MANY_TASKS      1							//错误任务
#define ERROR_SCH_CANNOT_DELETE_TASK  2							//任务无法删除

#define SCH_ENTER_CRITICAL()          {EA   =  0;}	//系统进入
#define SCH_EXIT_CRITICAL()           {EA   =  1;}	//系统退出

#define SCH_Start_Ticks()             {TR0   = 1;}	//启动T0定时

										   

typedef data struct 			      //定义在data区只有一个目的：快速存取
{
   void  (code * pTask)(void);    	  //任务函数指针
   INT32U Delay_MS;       	          //单位是ms
   INT16U Period_TICKS;               //单位是时标间隔(TICKS_TIME_MS)
   INT8U  RunMe;				      				//运行就绪标志
   INT8U  Preemptive_En;              //抢占性标志
   
}sTask; 

INT8U  SCH_Add_Task    (void (code *) (void), const INT32U, const INT16U, const INT8U);		//系统添加任务
INT8U  SCH_Delete_Task (const INT8U);	//系统删除任务				
void   SCH_Clear_Task  (const INT8U); //系统清空任务
void   SCH_Start_Tasks (void); 				//系统开始任务

#endif
                              
/*------------------------------------------------------------------*-
-*--------------------     END OF FILE     ---------------------------
-*------------------------------------------------------------------*/