/*--------------------------------------*-
文件：Sch51.h
时间：2016年3月30日
作者：HWY
版本：V1.0
-*---------------------------------------*/

#include "includes.h"

sTask SCH_tasks[SCH_MAX_TASKS];
INT8U Error_code = 0;	         //错误代码全局变量，可用于系统扩展

//=======================================================================================================================
void SCH_Clear_Task(const INT8U Index)	//清空第Index个任务块
{
   SCH_tasks[Index].pTask          = NULL;
   SCH_tasks[Index].Delay_MS       = 0;
   SCH_tasks[Index].Period_TICKS   = 0;
   SCH_tasks[Index].Preemptive_En  = 0;
   SCH_tasks[Index].RunMe          = 0;
}
//=======================================================================================================================
INT8U SCH_Add_Task(void (code * pFunction)(void), const INT32U DELAY, const INT16U PERIOD, const INT8U PREEMPTIVE_EN)    
{
   INT8U Index = 0;	 // Index最大为255，所以SCH_MAX_TASKS最大为256	
   
   /* 首先在队列中找到一个空块(如果有的话) */
   while((SCH_tasks[Index].pTask != NULL) && (Index < SCH_MAX_TASKS))
   {
      Index++;
   }   
   if(Index == SCH_MAX_TASKS)  //到达队尾
   {
      Error_code = ERROR_SCH_TOO_MANY_TASKS;	//错误任务
      return SCH_MAX_TASKS;
   }											   

   //如果运行到这，说明任务队列中有空间
   SCH_Clear_Task(Index);	       //先清空，再赋值

   SCH_tasks[Index].pTask          = pFunction; 
   SCH_tasks[Index].Delay_MS       = DELAY;
   SCH_tasks[Index].Period_TICKS   = PERIOD;
   SCH_tasks[Index].Preemptive_En  = PREEMPTIVE_EN;
   SCH_tasks[Index].RunMe          = 0;

   return Index; 
}

//=======================================================================================================================
INT8U SCH_Delete_Task(const INT8U TASK_ID) //reentrant
{													  
   INT8U Return_code;

   if(SCH_tasks[TASK_ID].pTask == NULL)	 //这里没有任务...
   {
      Error_code  = ERROR_SCH_CANNOT_DELETE_TASK;	//设置全局错误变量
      Return_code = RETURN_ERROR;
   }
   else	 //这里有任务...
   {
      Return_code = RETURN_NORMAL;			//返回正常
			SCH_tasks[TASK_ID].pTask = NULL;      
   }
   
   return Return_code;  //返回任务状态
}

//=======================================================================================================================
static void SCH_Go_To_Sleep()  //在SCH_Start_Tasks()末尾处进入空闲模式，当CPU收到任何中断时返回正常模式
{
   PCON |= 0x01;        
}

//=======================================================================================================================
void SCH_Start_Tasks(void) 	  //此函数只调度合作式任务，抢占式任务在T0的ISR中调度
{
   INT8U Index;	
   	
   SCH_Start_Ticks();

   while(1)	  //一旦启动多任务调度将永不返回
   {																 
	   for(Index = 0; Index < SCH_MAX_TASKS; Index++)  
	   {
	      if((SCH_tasks[Index].Preemptive_En == 0) && (SCH_tasks[Index].RunMe > 0)) //如果当前任务是合作式任务且已就绪
	      {
	         (*SCH_tasks[Index].pTask)();  	//运行该任务
	         SCH_tasks[Index].RunMe -= 1;   
	
	         if(SCH_tasks[Index].Period_TICKS == 0)  //如果这是个“单次”任务，将它从队列中删除
					 {
									 SCH_Delete_Task(Index);
					 } //否则周期性的任务将继续运行
	      }  
	   }

	   SCH_Go_To_Sleep();    //调度器进入空闲模式
   }        
}

/*------------------------------------------------------------------*-
-*--------------------     END OF FILE     ---------------------------
-*------------------------------------------------------------------*/