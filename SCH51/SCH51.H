/*--------------------------------------*-
�ļ���SCH51.H
ʱ�䣺2016��3��30��
���ߣ�HWY
�汾��V1.0
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
#define FALSE                         0							//��
#define TRUE                          (!FALSE)			//��
#endif

#define RETURN_NORMAL                 0							//��������
#define RETURN_ERROR                  1							//���ش���

#define INTERRUPT_EXTERNAL_0          0							//�ⲿ�ж�0
#define INTERRUPT_TIM_0_OF            1	            //OF == OverFlow(���)
#define INTERRUPT_EXTERNAL_1          2							//�ⲿ�ж�1
#define INTERRUPT_TIM_1_OF            3
#define INTERRUPT_UART_RX_TX          4							//�����ж�
#define INTERRUPT_TIM_2_OF            5

#define ERROR_SCH_TOO_MANY_TASKS      1							//��������
#define ERROR_SCH_CANNOT_DELETE_TASK  2							//�����޷�ɾ��

#define SCH_ENTER_CRITICAL()          {EA   =  0;}	//ϵͳ����
#define SCH_EXIT_CRITICAL()           {EA   =  1;}	//ϵͳ�˳�

#define SCH_Start_Ticks()             {TR0   = 1;}	//����T0��ʱ

										   

typedef data struct 			      //������data��ֻ��һ��Ŀ�ģ����ٴ�ȡ
{
   void  (code * pTask)(void);    	  //������ָ��
   INT32U Delay_MS;       	          //��λ��ms
   INT16U Period_TICKS;               //��λ��ʱ����(TICKS_TIME_MS)
   INT8U  RunMe;				      				//���о�����־
   INT8U  Preemptive_En;              //��ռ�Ա�־
   
}sTask; 

INT8U  SCH_Add_Task    (void (code *) (void), const INT32U, const INT16U, const INT8U);		//ϵͳ��������
INT8U  SCH_Delete_Task (const INT8U);	//ϵͳɾ������				
void   SCH_Clear_Task  (const INT8U); //ϵͳ�������
void   SCH_Start_Tasks (void); 				//ϵͳ��ʼ����

#endif
                              
/*------------------------------------------------------------------*-
-*--------------------     END OF FILE     ---------------------------
-*------------------------------------------------------------------*/