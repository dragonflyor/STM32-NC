# STM32-NC
STM32 NC


###1.数据协议
---
#####1.1 接收到数据头
 	>>01+文件名字符串:文件开始传输，保存在默认
 		">>01+filename"->收文件
 	>>02+文件名字符串:文件开始传输，传输完成后就执行
 		">>02+filename"->收文件->执行文件
 	$$01:文件传输结束

###2.串口状态条尾部状态信号

	//状态条尾部蓝绿色闪烁：表示系统串口任务正在接收数据
	LCD_Fill(lcddev.width-19,lcddev.height-39,lcddev.width,lcddev.height-21,lcd_discolor[10-rec_count%2]); //填充状态信号

---
	//状态条尾部灰，橙色闪烁表示系统串口任务正在轮询工作
	LCD_Fill(lcddev.width-19,lcddev.height-39,lcddev.width,lcddev.height-21,lcd_discolor[13-com_num%2]); //填充状态信号
	
###3.超时后，文件接受终止
本次接收的文件将丢弃 不会保存在本地

	void m_creatFile(char * pathname ,int timeout,u16 cycletime)
	{
		……
		while(1)
		{
			……
			//超时退出
			if(t == timeout){
				printf("文件接收超时 \r\n");
				return;
			}
			//串口轮询时间与等于200ms
			delay_ms(cycletime);		 			   
			LED0=!LED0;
		} 
	}

###4.ucosIII中任务的删除
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

###4.ucosIII中任务的挂起和恢复
	while(1)
	{
		task1_num++;	//任务1执行次数加1 注意task1_num1加到255的时候会清零！！
		LED0= ~LED0;
		printf("任务1已经执行：%d次\r\n",task1_num);
		if(task1_num==5) 
		{
			OSTaskSuspend((OS_TCB*)&Task2_TaskTCB,&err);//任务1执行5次后挂起任务2
			printf("任务1挂起了任务2!\r\n");
		}
		if(task1_num==10) 
		{
			OSTaskResume((OS_TCB*)&Task2_TaskTCB,&err);	//任务1运行10次后恢复任务2
			printf("任务1恢复了任务2!\r\n");
		}
		LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
		LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//显示任务执行次数
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
		
	}

#### 5.进不了系统
在NCSupport.c中加

	_ttywrch(int ch)
	{
	ch = ch;
	}
use mcroLIb程序就无法进入ucos系统，mcrolib是不在操作系统下运行的库

	解决的代码如下: 
	//不使用半主机模式
	#if 1 //如果没有这段，则需要在target选项中选择使用USE microLIB
	#pragma import(__use_no_semihosting)    //注释本行, 方法1
	struct __FILE {
	int handle;
	};
	FILE __stdout;
	
	_sys_exit(int x)
	{
	x = x;
	}
	
	//__use_no_semihosting was requested, but _ttywrch was referenced, 增加如下函数, 方法2
	_ttywrch(int ch)
	{
	ch = ch;
	}
	#endif

####6.系统软件复位
#####6.1 F103
[http://blog.csdn.net/huangtonggao/article/details/6446208](http://blog.csdn.net/huangtonggao/article/details/6446208 "stm32软件复位")
	
	1）采用官方自带的软件库
	 
	  在官方软件库的stm32f10x_nvic.c 文件里 直接提供了 系统复位的函数
	 
	所以要系统复位 只要 调用这两个函数就OK了
	NVIC_SETFAULTMASK（）；
	GenerateSystemReset（）；

#####6.1 F407 软件复位
执行如下代码

	#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)
	#define SYSRESETREQ_MASK (2<<1)
	SCB->AIRCR = AIRCR_VECTKEY_MASK | SYSRESETREQ_MASK;