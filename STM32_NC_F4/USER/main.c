#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include <string.h>
#include "malloc.h"  
#include "sdio_sdcard.h" 

#include "NCSupport.h"

int main(void)
{
	u8 t=0; 
	char *fliename;
	FIL readFile;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	//M3!=M4
	delay_init(168);	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为9600
 	exfuns_init();		//为fatfs相关变量申请内存				 
	LCD_Init();			//初始化液晶 
	LED_Init();         //LED初始化	 
	KEY_Init();		    //按键的初始化
	//M4+
	my_mem_init(SRAMIN);
	


//	driver_GPIOInit1( );
//	axisToZero1( );//回零

	LCD_ShowString(10,90,200,24,24,     "  Welcome to the");
 	LCD_ShowString(10,90+24*1,200,24,24,"  embeded system!");	 
	LCD_ShowString(10,90+24*2,200,16,16,"   Thanks for suporting!");	

	delay_ms(1000);
	//M4 modify
	while(SD_Init())
	{
		LCD_ShowString(60,150,200,16,16,"SD Card Error!");	//如果没检测到，提示错误
		printf("%X:SD Card Error!\r\n",SD_Init()); //提示错误的类型
		delay_ms(200);
		LCD_Fill(60,150,240,150+16,WHITE);//清除显示			  
		delay_ms(200);
		LED0=!LED0;//DS0闪烁
	}	
	
	exfuns_init();							//为fatfs相关变量申请内存				 
  f_mount(fs[0],"0:",1); 			//挂载SD卡 
	
	printf("start \r\n");
	
	
	fliename =mymalloc(SRAMIN,20);//动态分配空间
	memset(fliename,'\0',strlen(fliename));
	//带UI的文件选择
	selectFileFromSD(fliename);
	
	//获取的文件名
	printf(">>>>filemane:%s \r\n",fliename);
	
	//获取文件变量
	getFile(&readFile,fliename);
	myfree(SRAMIN,fliename);

	
	//带UI的执行文件
	nc_excuteFile(&readFile);
	
	while(1)
	{
		t++;
		delay_ms(200);		 			   
		LED0=!LED0;
	} 

}

