/*--------------------------------------*-
�ļ���	T0_uart_init.c
ʱ�䣺2016��3��30��
���ߣ�HWY
�汾��V1.0
-*---------------------------------------*/

#include "includes.h"

#define OSC_FREQ_DIV  12   //����12��Ƶ(51��Ƭ����ʱ�����־���Ϊϵͳ����12��Ƶ)

INT32U  MS_Counter  =  0;  //32λ�����������������ϵͳ��չ


extern  sTask SCH_tasks[SCH_MAX_TASKS];
extern  INT8U Error_code;

INT8U  _TH0_Init_Tick_Time = ( 65536 - (INT16U) ( OSC_FREQ * T0_TICK_TIME_MS / (OSC_FREQ_DIV * 1000) ) ) / 256;  //��ʱָ��ms 
INT8U  _TL0_Init_Tick_Time = ( 65536 - (INT16U) ( OSC_FREQ * T0_TICK_TIME_MS / (OSC_FREQ_DIV * 1000) ) ) % 256;  //��ʱָ��ms

//==============================================================================================================================
#if USE_DELAY_MS==1					//ʹ����ʱ����(��λ��ms)
void Delay_ms(INT32U N_ms)  //��λ��ms
{
	INT32U t_old, t_now, t_cnt = 0;

	t_old = MS_Counter;	    //����ս���ʱMS_Counter����ֵ

	while(1)
	{
		t_now = MS_Counter; //ʵʱ���浱ǰMS_Counter����ֵ

		if(t_now != t_old)
		{
			if(t_now > t_old)
				t_cnt += t_now - t_old;
			else
				t_cnt += 0xFFFFFFFF - (t_old - t_now);
		}

		t_old = t_now;	    //��һ��ܹؼ� 

		if(t_cnt >= N_ms)
			break;
	}
}
#endif

//==============================================================================================================================
static void SCH_Timer0_Reload(void)		//��ʱ�� T0��װ��
{
    TH0  = _TH0_Init_Tick_Time;   
    TL0  = _TL0_Init_Tick_Time;   
}

//==============================================================================================================================
void SCH_Init_T0_UART(void) 				//T0��UART��ʼ������
{
    INT8U Index;

    for(Index = 0; Index < SCH_MAX_TASKS; Index++) 
    {
       SCH_Clear_Task(Index);		//�������
    }

    Error_code = 0;

	//== T0��UART��ʼ������ ====================================================================================================
	TMOD  = 0x21;		 //T1:Gate C/T M1 M0 T0:Gate C/T M1 M0
                   //    0    0  1  0      0    0  0  1�� T1��ʽ2��ʱ��8λ��ֵ��װ��T0:��ʱ��,��ʽ1(16λ)
	SCH_Timer0_Reload();   //��ʱ�� T0��װ�� 
    
	SCON  = 0x40;        //SCON=0100 0000B�������Է�ʽ1����, 8λ�����ʿɱ�, 1��ʼλ8����λ1ֹͣλ
  
  #if USE_UART ==1	     //ʹ�ô���
  #if BAUDRATE == 9600UL
	  PCON  = 0x00;      //PCON=0000 0000B�������ʲ�����,    ������9600,    ����11.0592 			
	  TH1   = 0xFd;      //�����ʣ�9600 bps
	  TL1   = 0xFd;
  #endif
	
  #if BAUDRATE == 57600UL
	  PCON  = 0x80; 
      TH1   = 0xFF;
	  TL1   = 0xFF;
  #endif		         //���ƵĿ����Զ������ó��ò����ʣ����У�UL����ʡ��,����������
  #endif /* @ #if USE_UART ==1 */

	EA    = 1;           //�����ж�(���뿪��)
	ET0   = 1;	         //����T0��ʱ�ж�(���뿪��)
  ET1   = 0;	         //��ֹT1��ʱ�ж�(�����ֹ����ΪT1�����ڲ���������)

  #if (USE_UART_RI==1 || USE_UART_TI==1)	 
	ES    = 1;           //���������ж�
  #else
   	ES    = 0; 			 //��ֹ�����ж�
  #endif

  #if USE_UART ==1	 	 //ʹ�ô���
    TR1   = 1;	         //T1��ʼ���ڶ�ʱ����������
  #else
    TR1   = 0;	         //��ֹT1���ڶ�ʱ����������
  #endif

    TR0   = 0;           //��ǰ��ֹT0��ʱ(ϵͳ����������ʱ�ٿ���)  	
}
																															   
//==============================================================================================================================
void SCH_Update(void)  interrupt  INTERRUPT_TIM_0_OF 	  //T0_TICK_TIME_MS(ms)��ʱ��
{
    INT8U Index;

    SCH_Timer0_Reload();		//��ʱ�� T0��װ�� 

		MS_Counter  += T0_TICK_TIME_MS ;		  //ÿT0_TICK_TIME_MSms����T0_TICK_TIME_MSms

    for(Index = 0; Index < SCH_MAX_TASKS; Index++)  
    {
       if(SCH_tasks[Index].pTask)	  //ע�⣬pTask�п���Ϊ�գ���ΪSCH_MAX_TASKS���Դ���ʵ�����ӵ�������
       {
          if(SCH_tasks[Index].Delay_MS <= 0)				  
          { 
						if(SCH_tasks[Index].Preemptive_En == 1) //Ϊ��ռʽ��������������
						{
							(*SCH_tasks[Index].pTask)();
							SCH_tasks[Index].RunMe -= 1;  //RunMe��־��λ/��1

							if(SCH_tasks[Index].Period_TICKS == 0)  //��������ռʽ�����Ǹ������Ρ����񣬽����Ӷ�����ɾ���������������
							{										 
								SCH_tasks[Index].pTask = NULL;  //SCH_Delete_Task(Index);    
							}	                                     											 	 
						}
						else	//Ϊ����ʽ����
						{
							SCH_tasks[Index].RunMe += 1;
						}
										
						if(SCH_tasks[Index].Period_TICKS)	 //Period != 0��ʾ������������ִ�У�����ִֻ��һ��
						{
							SCH_tasks[Index].Delay_MS = SCH_tasks[Index].Period_TICKS * TICKS_TIME_MS;	//���Ľ��ͼ�����˵��
						}
				 }										 
				 else									 
				 { 								     
					 SCH_tasks[Index].Delay_MS -= T0_TICK_TIME_MS;	 //��ȥ��ʱ����
				 }										 
      }         								 
    }											 
} 

//==============================================================================================================================
void SendOneByte(unsigned char c)   //ͨ�� ���ڷ������ݸ���λ��
{
    SBUF = c;
    while(!TI);
    TI = 0;
}

//==============================================================================================================================
#if (USE_UART_RI==1 || USE_UART_TI==1)	  //���ʹ�ô��ڽ����жϻ��ߴ��ڷ����ж�
void SCH_UART_INTERRUPT(void)  interrupt  INTERRUPT_UART_RX_TX 	  
{
	#if (USE_UART_RI==1)	//ʹ�ô��ڽ����ж�
    if(RI) 
    {
        RI = 0;

        /*������������Լ��Ĵ���*/

    }
	#endif

	#if (USE_UART_TI==1) //ʹ�ô��ڷ����ж�
    if(TI) 
    {
        TI = 0;

        /*������������Լ��Ĵ���*/
    }
	#endif
}
#endif


/*** ���� ˵�� ���� *****************************************************************************************/
/*       																	                                    */
/*       Period_TICKS֮��������ʱ����Ϊ��λ����������Delay_MS��T0_TICK_TIME_MS(ms)Ϊ��λ������Ϊ����ʱ��T0    */
/*   ��ʱ��׼Ĭ��Ϊ1msʱ��ʱ��������ȡֵ�ܴ�(65535)���������ţ���Ӱ�춨ʱ��T0�����á���Ϊ��ʱ����ʱ�����ֵ   */
/*   (50ms)���ơ�				                                                                                */
/*                                                                                                              */
/****************************************************************************************************************/


/*------------------------------------------------------------------*-
-*--------------------     END OF FILE     ---------------------------
-*------------------------------------------------------------------*/