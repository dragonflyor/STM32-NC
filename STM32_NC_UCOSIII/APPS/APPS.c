
#include "APPS.h"

/************************0.启动任务*start_task*************************/
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

/************************1.任务1*task1_task*************************/
//任务优先级
#define TASK1_TASK_PRIO		4
//任务堆栈大小	
#define TASK1_STK_SIZE 		128
//任务控制块
OS_TCB Task1_TaskTCB;
//任务堆栈	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

/************************2.任务2*task2_task*************************/
//任务优先级
#define TASK2_TASK_PRIO		5
//任务堆栈大小	
#define TASK2_STK_SIZE 		128
//任务控制块
OS_TCB Task2_TaskTCB;
//任务堆栈	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//任务函数
void task2_task(void *p_arg);


/************************3.串口任务*com_task*************************/
//任务优先级
#define COM_TASK_PRIO		6
//任务堆栈大小	
#define COM_STK_SIZE 		512
//任务控制块
OS_TCB COM_TaskTCB;
//任务堆栈	
CPU_STK COM_TASK_STK[COM_STK_SIZE];
//任务函数
void com_task(void *p_arg);


//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };


//开始任务任务函数
void start_task(void *p_arg)
{
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;

		CPU_Init();
	#if OS_CFG_STAT_TASK_EN > 0u
		 OSStatTaskCPUUsageInit(&err);  	//统计任务                
	#endif
		
	#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
			CPU_IntDisMeasMaxCurReset();	
	#endif
		
	#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
		 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
		OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
	#endif	
		
		OS_CRITICAL_ENTER();	//进入临界区
		//创建TASK1任务
		OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
					 (CPU_CHAR	* )"Task1 task", 		
									 (OS_TASK_PTR )task1_task, 			
									 (void		* )0,					
									 (OS_PRIO	  )TASK1_TASK_PRIO,     
									 (CPU_STK   * )&TASK1_TASK_STK[0],	
									 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
									 (CPU_STK_SIZE)TASK1_STK_SIZE,		
									 (OS_MSG_QTY  )0,					
									 (OS_TICK	  )0,					
									 (void   	* )0,					
									 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
									 (OS_ERR 	* )&err);				
					 
		//创建TASK2任务
		OSTaskCreate((OS_TCB 	* )&Task2_TaskTCB,		
					 (CPU_CHAR	* )"task2 task", 		
									 (OS_TASK_PTR )task2_task, 			
									 (void		* )0,					
									 (OS_PRIO	  )TASK2_TASK_PRIO,     	
									 (CPU_STK   * )&TASK2_TASK_STK[0],	
									 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
									 (CPU_STK_SIZE)TASK2_STK_SIZE,		
									 (OS_MSG_QTY  )0,					
									 (OS_TICK	  )0,					
									 (void   	* )0,				
									 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
									 (OS_ERR 	* )&err);			 
			
/*
								 //任务优先级
#define COM_TASK_PRIO		6
//任务堆栈大小	
#define COM_STK_SIZE 		128
//任务控制块
OS_TCB COM_TaskTCB;
//任务堆栈	
CPU_STK COM_TASK_STK[COM_STK_SIZE];
//任务函数
void com_task(void *p_arg);
								 */								 
	//创建串口任务
		OSTaskCreate((OS_TCB 	* )&COM_TaskTCB,		
				 (CPU_CHAR	* )"com task", 		
                 (OS_TASK_PTR )com_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )COM_TASK_PRIO,     	
                 (CPU_STK   * )&COM_TASK_STK[0],	
                 (CPU_STK_SIZE)COM_STK_SIZE/10,	
                 (CPU_STK_SIZE)COM_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			 
	
								 
		OS_CRITICAL_EXIT();	//退出临界区
									 
		OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身							 
}


//task1任务函数
void task1_task(void *p_arg)
{
		u8 task1_num=0;
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;
		
		POINT_COLOR = BLACK;
		OS_CRITICAL_ENTER();
		LCD_DrawRectangle(5,110,115,314); 	//画一个矩形	
		LCD_DrawLine(5,130,115,130);		//画线
		POINT_COLOR = BLUE;
		LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
		OS_CRITICAL_EXIT();
		while(1)
		{
				task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
				LED0= ~LED0;
				printf("任务1已经执行：%d次\r\n",task1_num);
				if(task1_num==5) 
				{
						OSTaskDel((OS_TCB*)&Task2_TaskTCB,&err);	//任务1执行5此后删除掉任务2
						printf("任务1删除了任务2!\r\n");
				}
				LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
				LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//显示任务执行次数
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
			
		}
}

//task2任务函数
void task2_task(void *p_arg)
{
		u8 task2_num=0;
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;
		
		POINT_COLOR = BLACK;
		OS_CRITICAL_ENTER();
		LCD_DrawRectangle(125,110,234,314); //画一个矩形	
		LCD_DrawLine(125,130,234,130);		//画线
		POINT_COLOR = BLUE;
		LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
		OS_CRITICAL_EXIT();
		while(1)
		{
				task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
				LED1=~LED1;
				printf("任务2已经执行：%d次\r\n",task2_num);
				LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //显示任务执行次数
				LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
		}
}



//串口任务函数
void com_task(void *p_arg)
{
		/*
					//void mymemset(void *s,u8 c,u32 count);	//设置内存
					//void mymemcpy(void *des,void *src,u32 n);//复制内存  
		
		*/
		u8 com_num=0;
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;
		
		POINT_COLOR = BLACK;
		OS_CRITICAL_ENTER();
		
		LCD_DrawRectangle(125+120,110,234+120,314); //画一个矩形	
		LCD_DrawLine(125+120,130,234+120,130);		//画线
		POINT_COLOR = GRAY;
		LCD_ShowString(126+120,111,110,16,16,"com Run:000");
		
		
		LCD_DrawRectangle(1,lcddev.height-40,lcddev.width,lcddev.height-1); //屏幕底部画一个矩形	
		LCD_DrawLine(1,lcddev.height-20,lcddev.width,lcddev.height-20);		//画线
		LCD_ShowString(2,lcddev.height-39,500,16,16,"Received data from srialport:"); //标题
		
		OS_CRITICAL_EXIT();
		while(1)
		{
				com_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
				LED1=~LED1;
				printf("串口任务已经执行：%d次\r\n",com_num);
				LCD_ShowxNum(206+120,111,com_num,3,16,0x80);  //显示任务执行次数
				LCD_Fill(126+120,131,233+120,313,lcd_discolor[13-com_num%14]); //填充区域
				//状态条尾部灰，橙色闪烁表示系统串口任务正在轮询工作
				LCD_Fill(lcddev.width-19,lcddev.height-39,lcddev.width,lcddev.height-21,lcd_discolor[13-com_num%2]); //填充状态信号
	
				//-----------------------------------------------------------------------
				//成功接收到数据
				if(USART_RX_STA&0x8000)
				{
						//接收到的数据长度
						int len=USART_RX_STA&0x3fff;
						char * conn_recv;

						//开辟内存
						conn_recv = (char*)mymalloc(SRAMIN,len+1);
						mymemset(conn_recv,0,len+1);	//设置内存
						
						//取串口的数据 
						mymemcpy(conn_recv, USART_RX_BUF,len);
						
						//状态复位
						USART_RX_STA=0;
						
						//显示串口状态到屏幕
						com_statusBar(conn_recv);
						
						if(conn_recv[0]=='>' && conn_recv[1]=='>')
						{
								printf("收文件\r\n");
								m_creatFile("0:/NCCODE/conndata.txt",10,200);
								printf("结束收文件\r\n");
						}

						//释放内存
						myfree(SRAMIN,conn_recv); 	
				}
				//-----------------------------------------------------------------------
				OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
			
		}
}



