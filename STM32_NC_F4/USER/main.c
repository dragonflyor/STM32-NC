#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include <string.h>
#include "malloc.h"  
//#include "MMC_SD.h" 
#include "sdio_sdcard.h" 

#include "NCSupport.h"

int main(void)
{
	u8 t=0;
//	
//	 
//	char numTemp=0;	//?
//	char name[20]; //获取的文件名的保存数组
//	char *dir="0:/NCCODE";	//默认的初始文件目录
//	char dirtemp[20];//目录暂时存放
	 
//ADD by XZ 20160107
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
// ADDEND

	

//	 
//	NVIC_Configuration();  //中断配置	 
	//M3!=M4
	delay_init(168);	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为9600
 	exfuns_init();		//为fatfs相关变量申请内存				 
	LCD_Init();			//初始化液晶 
	LED_Init();         //LED初始化	 
	KEY_Init();		    //按键的初始化
	//M4+
	my_mem_init(SRAMIN);
	
	str=mymalloc(SRAMIN,96);

// 	mem_init();			//初始化内存池	
//	
//	driver_GPIOInit1( );
//	
//	//axisToZero1( );//回零
//	
	LCD_ShowString(10,90,200,24,24,     "  Welcome to the");
 	LCD_ShowString(10,90+24*1,200,24,24,"  embeded system!");	 
	LCD_ShowString(10,90+24*2,200,16,16,"   Thanks for suporting!");	
//	
	delay_ms(1000);
	//while(SD_Initialize())					//检测SD卡
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
	
//	
//	//窗口
//	LCD_Clear(BLACK);
//	LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//填充指定颜色
//	LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//填充指定颜色
//		//文本信息
//	Temp_X=INTERVAL+3;   
//	Temp_Y=INTERVAL+3;	 
//	//------
//	
//	memset(name,'\0',sizeof(name));	//	将数组NAME清空为0
//	
////	//选择数据放大倍数
////	NC_setZoom( );//放大比
//	
//	BACK_COLOR=YELLOW;//黄色背景
//	LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,"FILE DIRCTORY:-----------");
//	
//		while (get_fileName(name)==FR_OK)  // get_fileName读取文件名，遍历所有文件名
//	{
//		numTemp++;				 //	  统计文件数量
//		printf("F%-2d:%s \r\n",numTemp,name);
//		LCD_ShowString(Temp_X, Temp_Y+16*numTemp,8*9,16,16,(u8 *)name);
//		
//	}
//	LCD_ShowString(Temp_X, Temp_Y+16*(numTemp+1),8*30,16,16,"-------------------------");

//	memset(name,'\0',sizeof(name));
//	
//		//显示提示信息
//		//文本信息显示到LCD
//		Temp_X=INTERVAL+3;
//		Temp_Y=Temp_Y=lcddev.width+3;
//		BACK_COLOR=WHITE;
//		POINT_COLOR=BLACK;
//		LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,(u8 *)"THE CHOOSED FILE:---------");
//		LCD_ShowString(Temp_X, Temp_Y+16*1,8*30,16,16,(u8 *)"Pre Key0 to choose file! ");
//		LCD_ShowString(Temp_X, Temp_Y+16*2,8*30,16,16,(u8 *)"(KEY0 for changing file; ");
//		LCD_ShowString(Temp_X, Temp_Y+16*3,8*30,16,16,(u8 *)" KEY1 for confirming .)");
//	
//		while (*name==0)//当前文件名为空
//	{
//	select_file(name);	 //选文件
//	if (*name==0) printf("\n未正确选择文件\r\n");
//	}
//	
//printf("choose----->:%s\n",name);
//	

//	
//	
//	POINT_COLOR=RED;
//	LCD_ShowFrame();//绘制框架
//	//显示参数初值
////	LCD_UpdataShow(x,y,  	current_position[0],current_position[1],
////							destination[0],destination[1],
////								0,0);
//	updataShow(pNC_Para,0);//显示窗口数据
//	
//	printf("%s----------",dirtemp);	//调试时显示在串口
//	strcpy(dirtemp,dir);
//	strcat(dirtemp,"/");//得到绝对路径名称
//	strcat(dirtemp,name);
//	printf("%s----------",dirtemp);

//	res = f_open(&m_file, dirtemp, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);//读取的绝对目录文件到m_file
//	memset(dirtemp,'\0',sizeof(dirtemp));//释放内容
//  if(res != 0)
//        printf("Open File(%s) failed.\r\n",name);
//  else
//        printf("open Flie(%s) successed.\r\n",name); 
//         
//	
		selectFileFromSD();
//	pcode=mymalloc(SRAMIN,96);//动态分配空间
//	printf("\r\n-----------------EXpalianation  Begined!!!----------------------\r\n\r\n");		
//		
//		pGCode_Msg=(GCode_Msg*)malloc(sizeof(GCode_Msg));
//		pNC_Para=(NC_Para*)malloc(sizeof(NC_Para));
//		
//		while(((pcode=getALineCode(&m_file))!=NULL))
//		{ 
//#if USE_MAIN_DEBUG
//			printf("获取代码:%s\r\n",pcode);

//			//1译码
//			printf("译码\r\n");
//#endif
//			decode(pcode);
//			//printGCode_Msg(pGCode_Msg);
//		
//			
//			//2等待无任务
//#if USE_MAIN_DEBUG
//			printf("开始等待\r\n");
//#endif
//			while(pNC_Para->IS_INTERPOLATINO==1) LED1=0;//定时器正在驱动其他任务
//			LED1=1;//插补状态灯关闭
//			
//				//3装载参数
//#if USE_MAIN_DEBUG
//			printf("装载\r\n");
//#endif
//			prepare_interpolation(pGCode_Msg,pNC_Para);//保存到另一个结构体
//			
//			//updataShow(pNC_Para,0);
//			//显示到串口
//			
//			//printNCPara(pNC_Para);
//			
//			//4启动新任务
//#if USE_MAIN_DEBUG
//			printf("启动任务\r\n");
//#endif
//			interpolationChangeSpeed(1);//10ms
//			
//			//显示到LCD,使用显示会拖延时间
//			//显示模式0，全部插补数据刷新显示
//			updataShow(pNC_Para,0);
//			
//			interpolationTaskCreat();
//		}
//			
//		printf("\r\n\r\n-----------------EXpalianation  Finished!!!----------------------\r\n");
//		
//		myfree(SRAMIN,pcode);  //释放pcode的空间
//                  
//    f_close(&m_file);
//		
//  //LCD_PutString(0,64, "close successed."); 
// 	POINT_COLOR=BLUE;//设置字体为蓝色	  
//			BACK_COLOR=YELLOW;
//		LCD_ShowString(2*INTERVAL,2*INTERVAL,200,12,12,"CODE FINISH!");
	nc_excuteFile(&m_file);
	while(1)
	{
		t++;
		delay_ms(200);		 			   
		LED0=!LED0;
	} 

}

