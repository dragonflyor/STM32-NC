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

#include "ff.h"  
#include "exfuns.h"
#include "getAlineCodeFromSDtxt.h"
#include "fattester.h"
#include "interpolation.h"
#include "driverInterface.h"

 
#define INTERVAL 5  //LCD绘图的单位

//是否通过串口输出一些主函数执行信息   1：是  0：否
#define USE_MAIN_DEBUG (0)

u16 POINT_COLOR_LAST;//临时存放画笔颜色
//u16 BACK_COLOR_LAST;//临时存放背景颜色

FATFS m_fs;	//文件系统变量
FRESULT res;   //文件操作的结果返回值
FIL m_file;	 //文件变量
GCode_Msg * pGCode_Msg;
NC_Para * pNC_Para;
char* pcode;	//取一行的指针
extern u16 Temp_X,Temp_Y; //窗口坐标平移

u8 ZOOM=1;//把读取到的G代码的坐标尺寸放大ZOOM倍，便于调试

void LCD_ShowFrame(void);  //lCD显示窗口，初始化
FRESULT get_fileName(char *filenameTemp);  //通过按键选择文本名称
FRESULT select_file(char *filenameTemp); //选择文件


void LCD_ShowFrame(void){
	//设置窗口颜色
	LCD_Clear(BLACK);
	LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//填充指定颜色
	LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//填充指定颜色
	
	POINT_COLOR_LAST=POINT_COLOR;//保存画笔颜色
	POINT_COLOR=BLACK;//切换画笔颜色
	//外框
	LCD_DrawRectangle(INTERVAL/5, INTERVAL/5, lcddev.width-1-INTERVAL/5, lcddev.height-1-INTERVAL/5);
	//上边框
	LCD_DrawRectangle(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL);
	//下边框
	LCD_DrawRectangle(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL);
	
	//横坐标轴
	LCD_DrawLine(INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2, lcddev.width/2);
	//画X轴箭头
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2-INTERVAL*0.6);
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2+INTERVAL*0.6);
	//纵坐标轴
	LCD_DrawLine(lcddev.width/2, INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2);
	//Y轴箭头
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2-INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2+INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	
	POINT_COLOR=POINT_COLOR_LAST;//恢复画笔颜色
	
	//文本信息
	Temp_X=INTERVAL+3;
	Temp_Y=lcddev.width+3;
	LCD_ShowString(Temp_X, Temp_Y,8*9,16,16,"X :");
	LCD_ShowString(Temp_X+8*11, Temp_Y,6*9,16,16,"Y :");
	LCD_ShowString(Temp_X, Temp_Y+16,8*9,16,16,"X0:");
	LCD_ShowString(Temp_X+8*11, Temp_Y+16,8*9,16,16,"Y0:");
	LCD_ShowString(Temp_X, Temp_Y+16*2,8*9,16,16,"Xe:");
	LCD_ShowString(Temp_X+8*11, Temp_Y+16*2,8*9,16,16,"Ye:");
	LCD_ShowString(Temp_X, Temp_Y+16*3,8*9,16,16,"G :");
	LCD_ShowString(Temp_X+8*8, Temp_Y+16*3,8*9,16,16,"steps:");

}

////选择参数放大倍数

void NC_setZoom(void)
{
	char key;
	key=KEY_Scan(0);
	LCD_ShowString(Temp_X, Temp_Y,8*60,16,16,"PRES WK ZOOM Pra");
	LCD_ShowString(Temp_X, Temp_Y+16,8*60,16,16,"key1 for confirm");
		while(key!=KEY1_PRES)//不是确认键
		{
			if (key==WKUP_PRES)
			{
				LCD_ShowString(Temp_X, Temp_Y,8*20,16,16,"                   ");
				LCD_ShowString(Temp_X, Temp_Y+16,8*20,16,16,"                   ");
				ZOOM=(ZOOM<<1);//放大倍数乘以2
				if(ZOOM==0) ZOOM=1;//超出最大倍数 就重新从1开始
				LCD_ShowString(Temp_X, Temp_Y,8*9,16,16,"ZOOM:");
				LCD_ShowNum(Temp_X+8*11, Temp_Y,ZOOM,4,16); 
			}
			
			//key=getchar();
			key=KEY_NULL;
			key=KEY_Scan(0);
	}
}

////////////////////////////////////////////////////////////////////////////
////作用：从文件系统中选择文件
////输出：选定的文件的名称,FATFS没使用长文件格式，文件名应小于八个字符
////描述：s键进行选择，f键进行确认
////////////////////////////////////////////////////////////////////////////
FRESULT select_file(char *filenameTemp){
	FRESULT res;
	char key;
	char select_num=0;//选取的文件在目录中的位置
	//key=getchar();
	key=KEY_Scan(0);
	while(key!=KEY1_PRES){
		//字母‘s’进行选择 
		if (key==KEY0_PRES)
		{
			select_num++;
			//切换文件
			res=mf_select_file(filenameTemp,("0:/NCCODE"),select_num);
			if((res!=FR_OK)||(*filenameTemp=='\0')) {return FR_INVALID_NAME;}//选择错误
			
			//文本信息显示到LCD
		Temp_X=INTERVAL+3;
		Temp_Y=Temp_Y=lcddev.width+3;
			BACK_COLOR=WHITE;
			LCD_ShowString(Temp_X, Temp_Y+16*1,8*30,16,16,(u8 *)"                           ");//清空改行的显示
			LCD_ShowString(Temp_X, Temp_Y+16*1,8*9,16,16,(u8 *)filenameTemp);
		 	
		}
		
		//key=getchar();
		key=KEY_NULL;
		key=KEY_Scan(0);
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////
////作用：从文件系统中选择文件
////输出：选定的文件的名称,FATFS没使用长文件格式，文件名应小于八个字符
////返回：操作结果
////////////////////////////////////////////////////////////////////////////
FRESULT get_fileName(char *filenameTemp){
	FRESULT res;

	static char select_num=1;//选取的文件在目录中的位置

	res=mf_get_fileName(filenameTemp,"0:/NCCODE",select_num++);
	if((res!=FR_OK)||(*filenameTemp=='\0')) {return FR_INVALID_NAME;}//选择错误

	return res;
}




//void printNCPara(NC_Para *pNC_Para)
//{
//	int i=0;
// 	printf("print NC_Para----------\r\n");

//	for(i=0;i<3;i++)
//		printf("NC_Para->coor_currentValue[%d]: %d \r\n",i,pNC_Para->coor_currentValue[i]);
// 	for(i=0;i<3;i++)
// 		printf("NC_Para->coor_originValue[%d]:  %d \r\n",i,pNC_Para->coor_originValue[i]);
// 	for(i=0;i<3;i++)
// 		printf("NC_Para->coor_destinationValue[%d]:%d \r\n",i,pNC_Para->coor_destinationValue[i]);
//		
//		printf("NC_Para->f:%d \r\n",pNC_Para->f);
//		printf("NC_Para->G_instruct:%d \r\n",pNC_Para->G_instruct);
//		printf("NC_Para->IS_INTERPOLATINO:%d \r\n",pNC_Para->IS_INTERPOLATINO);
//		
//		printf("NC_Para->O_R:%d \r\n",pNC_Para->O_R);
//		printf("NC_Para->O_X:%f\r\n",pNC_Para->O_X);
//		printf("NC_Para->O_Y:%f \r\n",pNC_Para->O_Y);
//		printf("NC_Para->steps:%d\r\n",pNC_Para->steps);
//		
//}


//void printGCode_Msg(GCode_Msg * pGCode_Msg)
//{
//		int i=0;

// 	printf("print Gcode_Msg----------\r\n");
//	for(i=0;i<3;i++)
//		printf("pGCode_Msg->coor_value[%d]: %f \r\n",i,pGCode_Msg->coor_value[i]);
//	printf("pGCode_Msg->O_R: %f \r\n",pGCode_Msg->O_R);
//	
//	printf("pGCode_Msg->G_instruct: %d \r\n",pGCode_Msg->G_instruct);
//	printf("pGCode_Msg->F: %d \r\n",pGCode_Msg->F);
//	printf("pGCode_Msg->M_instruct: %d \r\n",pGCode_Msg->M_instruct);
//	
//	
//}


// //测试
// void test(void)
// {
// 	
// 	printf("test -------\r\n");
// 	
// 	pNC_Para->coor_originValue[0]=100;
// 	pNC_Para->coor_originValue[1]=-40;
// 	
// 	pNC_Para->coor_currentValue[0]=100;
// 	pNC_Para->coor_currentValue[0]=-40;
// 	
// 	pNC_Para->coor_destinationValue[0]=40;
// 	pNC_Para->coor_destinationValue[1]=-100;
// 	
// 	pNC_Para->G_instruct=0x03;
// 	
// 	pNC_Para->O_R=60;
// 	
// 	printf("test2 -------\r\n");
// 	printNCPara(pNC_Para);
// 	
// 	GetArcO1(pNC_Para);
// 	
// 	while(1);

// }


int main(void)
{
	u8 t=0;
	
	 
	char numTemp=0;	//?
	char name[20]; //获取的文件名的保存数组
	char *dir="0:/NCCODE";	//默认的初始文件目录
	char dirtemp[20];//目录暂时存放
	 
	

//ADD by XZ 20160107
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
// ADDEND

	
	str=mymalloc(SRAMIN,96);
//	 
//	NVIC_Configuration();  //中断配置	 
	//M3!=M4
	delay_init(168);	    	 //延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600
 	exfuns_init();		//为fatfs相关变量申请内存				 
	LCD_Init();			//初始化液晶 
	LED_Init();         //LED初始化	 
	KEY_Init();		    //按键的初始化
	//M4+
	my_mem_init(SRAMIN);

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
	

	
	
	//窗口
	LCD_Clear(BLACK);
	LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//填充指定颜色
	LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//填充指定颜色
		//文本信息
	Temp_X=INTERVAL+3;   
	Temp_Y=INTERVAL+3;	 
	//------
	
	memset(name,'\0',sizeof(name));	//	将数组NAME清空为0
	
//	//选择数据放大倍数
//	NC_setZoom( );//放大比
	
	BACK_COLOR=YELLOW;//黄色背景
	LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,"FILE DIRCTORY:-----------");
	
		while (get_fileName(name)==FR_OK)  // get_fileName读取文件名，遍历所有文件名
	{
		numTemp++;				 //	  统计文件数量
		printf("F%-2d:%s \r\n",numTemp,name);
		LCD_ShowString(Temp_X, Temp_Y+16*numTemp,8*9,16,16,(u8 *)name);
		
	}
	
	LCD_ShowString(Temp_X, Temp_Y+16*(numTemp+1),8*30,16,16,"-------------------------");

	memset(name,'\0',sizeof(name));
	
		//显示提示信息
		//文本信息显示到LCD
		Temp_X=INTERVAL+3;
		Temp_Y=Temp_Y=lcddev.width+3;
		BACK_COLOR=WHITE;
		POINT_COLOR=BLACK;
		LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,(u8 *)"THE CHOOSED FILE:---------");
		LCD_ShowString(Temp_X, Temp_Y+16*1,8*30,16,16,(u8 *)"Pre Key0 to choose file! ");
		LCD_ShowString(Temp_X, Temp_Y+16*2,8*30,16,16,(u8 *)"(KEY0 for changing file; ");
		LCD_ShowString(Temp_X, Temp_Y+16*3,8*30,16,16,(u8 *)" KEY1 for confirming .)");
	
		while (*name==0)//当前文件名为空
	{
	select_file(name);	 //选文件
	if (*name==0) printf("\n未正确选择文件\r\n");
	

	}
printf("choose----->:%s\n",name);
	

	
	
	POINT_COLOR=RED;
	LCD_ShowFrame();//绘制框架
	//显示参数初值
//	LCD_UpdataShow(x,y,  	current_position[0],current_position[1],
//							destination[0],destination[1],
//								0,0);
	updataShow(pNC_Para,0);//显示窗口数据
	
	
	
	
	printf("%s----------",dirtemp);	//调试时显示在串口
	strcpy(dirtemp,dir);
	strcat(dirtemp,"/");//得到绝对路径名称
	strcat(dirtemp,name);
	printf("%s----------",dirtemp);

	res = f_open(&m_file, dirtemp, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);//读取的绝对目录文件到m_file
	memset(dirtemp,'\0',sizeof(dirtemp));//释放内容
  if(res != 0)
        printf("Open File(%s) failed.\r\n",name);
    else
        printf("open Flie(%s) successed.\r\n",name); 
         
		
	pcode=mymalloc(SRAMIN,96);//动态分配空间
	printf("\r\n-----------------EXpalianation  Begined!!!----------------------\r\n\r\n");		
		
		pGCode_Msg=(GCode_Msg*)malloc(sizeof(GCode_Msg));
		pNC_Para=(NC_Para*)malloc(sizeof(NC_Para));
		
		
		
		
		while(((pcode=getALineCode(&m_file))!=NULL))
		{ 
#if USE_MAIN_DEBUG
			printf("获取代码:%s\r\n",pcode);

			//1译码
			printf("译码\r\n");
#endif
			decode(pcode);
			//printGCode_Msg(pGCode_Msg);
		
			
			//2等待无任务
#if USE_MAIN_DEBUG
			printf("开始等待\r\n");
#endif
			while(pNC_Para->IS_INTERPOLATINO==1) LED1=0;//定时器正在驱动其他任务
			LED1=1;//插补状态灯关闭
			
				//3装载参数
#if USE_MAIN_DEBUG
			printf("装载\r\n");
#endif
			prepare_interpolation(pGCode_Msg,pNC_Para);//保存到另一个结构体
			
			//updataShow(pNC_Para,0);
			//显示到串口
			
			//printNCPara(pNC_Para);
			
			//4启动新任务
#if USE_MAIN_DEBUG
			printf("启动任务\r\n");
#endif
			interpolationChangeSpeed(1);//10ms
			
			//显示到LCD,使用显示会拖延时间
			//显示模式0，全部插补数据刷新显示
			updataShow(pNC_Para,0);
			
			interpolationTaskCreat();
		}
			
		printf("\r\n\r\n-----------------EXpalianation  Finished!!!----------------------\r\n");
		
		myfree(SRAMIN,pcode);  //释放pcode的空间
                  
    f_close(&m_file);
		
  //LCD_PutString(0,64, "close successed."); 
 	POINT_COLOR=BLUE;//设置字体为蓝色	  
			BACK_COLOR=YELLOW;
		LCD_ShowString(2*INTERVAL,2*INTERVAL,200,12,12,"CODE FINISH!");
	
	while(1)
	{
		t++;
		delay_ms(200);		 			   
		LED0=!LED0;
	} 

}

