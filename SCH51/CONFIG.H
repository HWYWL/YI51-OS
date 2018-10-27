/*--------------------------------------*-
文件：config.h
时间：2016年3月30日
作者：HWY
版本：V1.0
-*---------------------------------------*/

#ifndef _CONFIG_H
#define _CONFIG_H

//========================================================
//=======  请在以下系统参数配置区配置系统相关参数  =======
//========================================================


#define SCH_MAX_TASKS    3		    //最大任务数

#define OSC_FREQ         11059200UL //单位：Hz(一般为：11059200UL 或 12000000UL)
									//如果使用串口，则OSC_FREQ选择11059200UL以产生精确波特率，
									//否则选择12000000UL产生精确定时

#define T0_TICK_TIME_MS  1		    //T0定时时长(单位：ms，取值范围：1-50，强烈推荐取值为1)
#define TICKS_TIME_MS    25	        //时标间隔(单位：ms)
//#define TICKS_TIME_MS    50	        //时标间隔(单位：ms)



#define USE_DELAY_MS     0 		    //不使用延时函数(单位：ms)


#define USE_UART         1 		    //使用串口
// #define USE_UART_RI      0 		    //不使用串口接收中断
// #define USE_UART_TI      0 		    //不使用串口发送中断
#define USE_UART_RI      1 		    //使用串口接收中断
#define USE_UART_TI      1 		    //使用串口发送中断
#define BAUDRATE         9600UL	//推荐使用9600UL和57600UL(优先推荐)
																	//MCU与PC联接时波特率要匹配




#endif

