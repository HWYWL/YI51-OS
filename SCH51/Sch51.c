/*--------------------------------------*-
�ļ���Sch51.h
ʱ�䣺2016��3��30��
���ߣ�HWY
�汾��V1.0
-*---------------------------------------*/

#include "includes.h"

sTask SCH_tasks[SCH_MAX_TASKS];
INT8U Error_code = 0;	         //�������ȫ�ֱ�����������ϵͳ��չ

//=======================================================================================================================
void SCH_Clear_Task(const INT8U Index)	//��յ�Index�������
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
   INT8U Index = 0;	 // Index���Ϊ255������SCH_MAX_TASKS���Ϊ256	
   
   /* �����ڶ������ҵ�һ���տ�(����еĻ�) */
   while((SCH_tasks[Index].pTask != NULL) && (Index < SCH_MAX_TASKS))
   {
      Index++;
   }   
   if(Index == SCH_MAX_TASKS)  //�����β
   {
      Error_code = ERROR_SCH_TOO_MANY_TASKS;	//��������
      return SCH_MAX_TASKS;
   }											   

   //������е��⣬˵������������пռ�
   SCH_Clear_Task(Index);	       //����գ��ٸ�ֵ

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

   if(SCH_tasks[TASK_ID].pTask == NULL)	 //����û������...
   {
      Error_code  = ERROR_SCH_CANNOT_DELETE_TASK;	//����ȫ�ִ������
      Return_code = RETURN_ERROR;
   }
   else	 //����������...
   {
      Return_code = RETURN_NORMAL;			//��������
			SCH_tasks[TASK_ID].pTask = NULL;      
   }
   
   return Return_code;  //��������״̬
}

//=======================================================================================================================
static void SCH_Go_To_Sleep()  //��SCH_Start_Tasks()ĩβ���������ģʽ����CPU�յ��κ��ж�ʱ��������ģʽ
{
   PCON |= 0x01;        
}

//=======================================================================================================================
void SCH_Start_Tasks(void) 	  //�˺���ֻ���Ⱥ���ʽ������ռʽ������T0��ISR�е���
{
   INT8U Index;	
   	
   SCH_Start_Ticks();

   while(1)	  //һ��������������Ƚ���������
   {																 
	   for(Index = 0; Index < SCH_MAX_TASKS; Index++)  
	   {
	      if((SCH_tasks[Index].Preemptive_En == 0) && (SCH_tasks[Index].RunMe > 0)) //�����ǰ�����Ǻ���ʽ�������Ѿ���
	      {
	         (*SCH_tasks[Index].pTask)();  	//���и�����
	         SCH_tasks[Index].RunMe -= 1;   
	
	         if(SCH_tasks[Index].Period_TICKS == 0)  //������Ǹ������Ρ����񣬽����Ӷ�����ɾ��
					 {
									 SCH_Delete_Task(Index);
					 } //���������Ե����񽫼�������
	      }  
	   }

	   SCH_Go_To_Sleep();    //�������������ģʽ
   }        
}

/*------------------------------------------------------------------*-
-*--------------------     END OF FILE     ---------------------------
-*------------------------------------------------------------------*/