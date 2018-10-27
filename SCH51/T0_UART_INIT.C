/*--------------------------------------*-
文件：	T0_uart_init.c
时间：2016年3月30日
作者：HWY
版本：V1.0
-*---------------------------------------*/

#include "includes.h"

#define OSC_FREQ_DIV  12   //晶振12分频(51单片机定时器部分晶振为系统晶振12分频)

INT32U  MS_Counter  =  0;  //32位毫秒计数器，可用于系统扩展


extern  sTask SCH_tasks[SCH_MAX_TASKS];
extern  INT8U Error_code;

INT8U  _TH0_Init_Tick_Time = ( 65536 - (INT16U) ( OSC_FREQ * T0_TICK_TIME_MS / (OSC_FREQ_DIV * 1000) ) ) / 256;  //定时指定ms 
INT8U  _TL0_Init_Tick_Time = ( 65536 - (INT16U) ( OSC_FREQ * T0_TICK_TIME_MS / (OSC_FREQ_DIV * 1000) ) ) % 256;  //定时指定ms

//==============================================================================================================================
#if USE_DELAY_MS==1					//使用延时函数(单位：ms)
void Delay_ms(INT32U N_ms)  //单位：ms
{
	INT32U t_old, t_now, t_cnt = 0;

	t_old = MS_Counter;	    //保存刚进入时MS_Counter的数值

	while(1)
	{
		t_now = MS_Counter; //实时保存当前MS_Counter的数值

		if(t_now != t_old)
		{
			if(t_now > t_old)
				t_cnt += t_now - t_old;
			else
				t_cnt += 0xFFFFFFFF - (t_old - t_now);
		}

		t_old = t_now;	    //这一句很关键 

		if(t_cnt >= N_ms)
			break;
	}
}
#endif

//==============================================================================================================================
static void SCH_Timer0_Reload(void)		//定时器 T0重装载
{
    TH0  = _TH0_Init_Tick_Time;   
    TL0  = _TL0_Init_Tick_Time;   
}

//==============================================================================================================================
void SCH_Init_T0_UART(void) 				//T0和UART初始化程序
{
    INT8U Index;

    for(Index = 0; Index < SCH_MAX_TASKS; Index++) 
    {
       SCH_Clear_Task(Index);		//清空任务
    }

    Error_code = 0;

	//== T0和UART初始化程序 ====================================================================================================
	TMOD  = 0x21;		 //T1:Gate C/T M1 M0 T0:Gate C/T M1 M0
                   //    0    0  1  0      0    0  0  1： T1方式2定时，8位初值重装，T0:定时器,方式1(16位)
	SCH_Timer0_Reload();   //定时器 T0重装载 
    
	SCON  = 0x40;        //SCON=0100 0000B，串口以方式1工作, 8位波特率可变, 1起始位8数据位1停止位
  
  #if USE_UART ==1	     //使用串口
  #if BAUDRATE == 9600UL
	  PCON  = 0x00;      //PCON=0000 0000B，波特率不倍增,    波特率9600,    晶振11.0592 			
	  TH1   = 0xFd;      //波特率：9600 bps
	  TL1   = 0xFd;
  #endif
	
  #if BAUDRATE == 57600UL
	  PCON  = 0x80; 
      TH1   = 0xFF;
	  TL1   = 0xFF;
  #endif		         //类似的可以自定义设置常用波特率，其中，UL不能省略,代表长整型
  #endif /* @ #if USE_UART ==1 */

	EA    = 1;           //开总中断(必须开启)
	ET0   = 1;	         //开启T0定时中断(必须开启)
  ET1   = 0;	         //禁止T1定时中断(必须禁止，因为T1以用于产生波特率)

  #if (USE_UART_RI==1 || USE_UART_TI==1)	 
	ES    = 1;           //开启串口中断
  #else
   	ES    = 0; 			 //禁止串口中断
  #endif

  #if USE_UART ==1	 	 //使用串口
    TR1   = 1;	         //T1开始串口定时产生波特率
  #else
    TR1   = 0;	         //禁止T1串口定时产生波特率
  #endif

    TR0   = 0;           //当前禁止T0定时(系统启动多任务时再开启)  	
}
																															   
//==============================================================================================================================
void SCH_Update(void)  interrupt  INTERRUPT_TIM_0_OF 	  //T0_TICK_TIME_MS(ms)定时到
{
    INT8U Index;

    SCH_Timer0_Reload();		//定时器 T0重装载 

		MS_Counter  += T0_TICK_TIME_MS ;		  //每T0_TICK_TIME_MSms自增T0_TICK_TIME_MSms

    for(Index = 0; Index < SCH_MAX_TASKS; Index++)  
    {
       if(SCH_tasks[Index].pTask)	  //注意，pTask有可能为空，因为SCH_MAX_TASKS可以大于实际添加的任务数
       {
          if(SCH_tasks[Index].Delay_MS <= 0)				  
          { 
						if(SCH_tasks[Index].Preemptive_En == 1) //为抢占式任务，则立即运行
						{
							(*SCH_tasks[Index].pTask)();
							SCH_tasks[Index].RunMe -= 1;  //RunMe标志复位/减1

							if(SCH_tasks[Index].Period_TICKS == 0)  //如果这个抢占式任务是个“单次”任务，将它从队列中删除，否则继续运行
							{										 
								SCH_tasks[Index].pTask = NULL;  //SCH_Delete_Task(Index);    
							}	                                     											 	 
						}
						else	//为合作式任务
						{
							SCH_tasks[Index].RunMe += 1;
						}
										
						if(SCH_tasks[Index].Period_TICKS)	 //Period != 0表示该任务周期性执行，否则，只执行一次
						{
							SCH_tasks[Index].Delay_MS = SCH_tasks[Index].Period_TICKS * TICKS_TIME_MS;	//这句的解释见下面说明
						}
				 }										 
				 else									 
				 { 								     
					 SCH_tasks[Index].Delay_MS -= T0_TICK_TIME_MS;	 //减去定时步长
				 }										 
      }         								 
    }											 
} 

//==============================================================================================================================
void SendOneByte(unsigned char c)   //通过 串口发生数据给上位机
{
    SBUF = c;
    while(!TI);
    TI = 0;
}

//==============================================================================================================================
#if (USE_UART_RI==1 || USE_UART_TI==1)	  //如果使用串口接收中断或者串口发送中断
void SCH_UART_INTERRUPT(void)  interrupt  INTERRUPT_UART_RX_TX 	  
{
	#if (USE_UART_RI==1)	//使用串口接收中断
    if(RI) 
    {
        RI = 0;

        /*这里可以添加自己的代码*/

    }
	#endif

	#if (USE_UART_TI==1) //使用串口发送中断
    if(TI) 
    {
        TI = 0;

        /*这里可以添加自己的代码*/
    }
	#endif
}
#endif


/*** ★★★ 说明 ★★★ *****************************************************************************************/
/*       																	                                    */
/*       Period_TICKS之所以是以时标间隔为单位而不采用像Delay_MS以T0_TICK_TIME_MS(ms)为单位，是因为当定时器T0    */
/*   定时基准默认为1ms时，时标间隔可以取值很大(65535)，这样安排，不影响定时器T0的设置。因为定时器定时有最大值   */
/*   (50ms)限制。				                                                                                */
/*                                                                                                              */
/****************************************************************************************************************/


/*------------------------------------------------------------------*-
-*--------------------     END OF FILE     ---------------------------
-*------------------------------------------------------------------*/