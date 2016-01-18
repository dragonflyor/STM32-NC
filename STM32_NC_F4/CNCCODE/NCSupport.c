#include "NCSupport.h"

#include "fattester.h"	 
#include "sdio_sdcard.h"
#include "usmart.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "ff.h"
#include "string.h"
#include "key.h"
#include "led.h"

#include "getAlineCodeFromSDtxt.h"
#include "driverInterface.h"




//是否通过串口输出一些主函数执行信息   1：是  0：否
#define USE_MAIN_DEBUG (0)

u16 POINT_COLOR_LAST;//临时存放画笔颜色
u8 ZOOM=1;//把读取到的G代码的坐标尺寸放大ZOOM倍，便于调试
u16 Temp_X,Temp_Y; //窗口坐标平移

GCode_Msg * pGCode_Msg;
NC_Para * pNC_Para;
char* pcode;	//取一行的指针


//加入以下代码,支持UCOS,而不需要选择use MicroLIB	 
_ttywrch(int ch)
{
ch = ch;
}

//初始化显示数据，数据格式为5位的整形
void LCD_UpdataShow( u32 x,u32 y,u32 x0,u32 y0,u32 xe,u32 ye,u32 g,u32 steps){
	//LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
	LCD_ShowNum(Temp_X+8*3, Temp_Y,x,6,16);
	LCD_ShowNum(Temp_X+8*14, Temp_Y,y,6,16);
		LCD_ShowNum(Temp_X+8*3, Temp_Y+16,x0,6,16);
	LCD_ShowNum(Temp_X+8*14, Temp_Y+16,y0,6,16);
		LCD_ShowNum(Temp_X+8*3, Temp_Y+16*2,xe,6,16);
	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*2,ye,6,16);
		LCD_ShowNum(Temp_X+8*3, Temp_Y+16*3,g,3,16);
	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,steps,6,16);
}



//////////////////////////////////////////////////////////////////////////
//BY XZ 20150428
//////////////////////////////////////////////////////////////////////////
//选择文件
//path:路径
//offset 首个文件向下偏移offset个
//返回值:执行结果
FRESULT mf_select_file(char * filename,char * path,char offset)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
	//FILINFO fileinfoTemp;	//头文件信息临时变量
#if S_MAX_SIZ
	fileinfo.fsize = S_MAX_SIZ * 2 + 1;
	fileinfo.fname ;
#endif		  
	
    res = f_opendir(&dir,(const char*)path); //打开一个目录

// 	if (f_readdir(&dir, &fileinfoTemp)!=FR_OK)                 //读取目录下的一个文件)
// 	{
// 		printf("\n文件错误\n");
// 	}
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(offset--)
		{
			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
			if (res != FR_OK || fileinfo.fname[0] == 0) 
			{ 
				res = f_opendir(&dir,(const char*)path); //打开一个目录
				if (res == FR_OK) offset++;continue;
				//printf("文件末尾");
				//break; 
			}//错误了/到末尾了,退出
			
			//if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if S_MAX_SIZ
			fn = *fileinfo.fname ? fileinfo.fname : fileinfo.fname;
#else							   
			fn = fileinfo.fname;
#endif	                                              /* It is a file. */

		} 
		//printf("%s/", path);//打印路径	
		//	printf("%s\n",  fn);//打印文件名	
				if(*fn!='\0')
		{
				printf("%s\n",  fn);//打印文件名
			strcpy(filename,fn);//输出文件
		}
		else
		{
			//	strcpy(filename,"is null");
			*filename='\0';
		}
    }	  
	//	myfree(fileinfo.lfname);
    return res;	  
}


//////////////////////////////////////////////////////////////////////////
//BY XZ 20150428
//////////////////////////////////////////////////////////////////////////
//得到目录首地址向下偏移位置的文件名
//path:路径
//offset 首个文件向下偏移offset个
//返回值:执行结果
FRESULT mf_get_fileName(char * filename,char * path,char offset)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
	//FILINFO fileinfoTemp;	//头文件信息临时变量
#if S_MAX_SIZ
	fileinfo.fsize = S_MAX_SIZ * 2 + 1;
	fileinfo.fname ;
#endif		  
	
    res = f_opendir(&dir,(const char*)path); //打开一个目录
	
	// 	if (f_readdir(&dir, &fileinfoTemp)!=FR_OK)                 //读取目录下的一个文件)
	// 	{
	// 		printf("\n文件错误\n");
	// 	}
    if (res == FR_OK) 
	{	
		//printf("\r\n"); 
		while(offset--)
		{
			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
			if (res != FR_OK || fileinfo.fname[0] == 0) //没有正确读取到文件，错误或者到了结尾
			{ 
				//res = f_opendir(&dir,(const char*)path); //到了结尾，再次打开这个目录开始位置
				//if (res == FR_OK) offset++;continue;//重新向下进行一次读取
				if (res != FR_OK )
				{
				//	printf("\n读取错误\n");
				}
				else
				{
				//	printf("\n文件末尾\n");
				}
				break; 
			}//错误了/到末尾了
			
			//if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if S_MAX_SIZ
			fn = *fileinfo.fname ? fileinfo.fname : fileinfo.fname;
#else							   
			fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			
		} 
	
		if(*fn!='\0')
		{
		//	printf("%s\n",  fn);//打印文件名
			strcpy(filename,fn);
		}
		else
		{

			*filename='\0';
		}

    }	  

    return res;	  
}

/*
加工轨迹UI
*/
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


/*
选文件的UI
第一个参数：选择的文件名称 的保存地址
*/
void selectFileFromSD(char * filename)
{
		char numTemp=0;	//选择到的文件在目录中的位置
		char name[20]; //获取的文件名的保存数组
		//	
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
		//返回文件名
		memcpy(filename,name,strlen(name)+1);

}

//通过文件名获取文件变量
//第一参数：输出的文件
//第二个参数：文件名（默认在SD卡:/NCCODE目录下）
FRESULT getFile(FIL * outFile,char *filename)
{
		FRESULT res;
		char *default_dir="0:/NCCODE";	//默认的初始文件目录
		char dirtemp[20];//目录暂时存放
	
		POINT_COLOR=RED;
		LCD_ShowFrame();//绘制框架
		updataShow(pNC_Para,0);//显示窗口数据
	
	printf("%s----------",dirtemp);	//调试时显示在串口
	strcpy(dirtemp,default_dir);
	strcat(dirtemp,"/");//得到绝对路径名称
	strcat(dirtemp,filename);
	printf("%s----------",dirtemp);
	res = f_open(outFile, dirtemp, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);//读取的绝对目录文件到m_file
	memset(dirtemp,'\0',sizeof(dirtemp));//释放内容
  if(res != 0)
        printf("Open File(%s) failed.\r\n",filename);
  else
        printf("open Flie(%s) successed.\r\n",filename); 
	return res;
}

//执行传入的文件
void nc_excuteFile(FIL *m_file )
{
		str=mymalloc(SRAMIN,96);
		pcode=mymalloc(SRAMIN,96);//动态分配空间
		printf("\r\n-----------------EXpalianation  Begined!!!----------------------\r\n\r\n");		
		
		pGCode_Msg=(GCode_Msg*)malloc(sizeof(GCode_Msg));
		pNC_Para=(NC_Para*)malloc(sizeof(NC_Para));
		
		while(((pcode=getALineCode(m_file))!=NULL))
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
                  
    f_close(m_file);
		
  //LCD_PutString(0,64, "close successed."); 
 	POINT_COLOR=BLUE;//设置字体为蓝色	  
			BACK_COLOR=YELLOW;
		LCD_ShowString(2*INTERVAL,2*INTERVAL,200,12,12,"CODE FINISH!");
	
} 

//插补数据显示到串口
//只在程序调试时候使用，因为输出这么多串口内容 会占用太长的CPU时间
void printNCPara(NC_Para *pNC_Para)
{
	int i=0;
 	printf("print NC_Para----------\r\n");

	for(i=0;i<3;i++)
		printf("NC_Para->coor_currentValue[%d]: %ld \r\n",i,pNC_Para->coor_currentValue[i]);
 	for(i=0;i<3;i++)
 		printf("NC_Para->coor_originValue[%d]:  %ld \r\n",i,pNC_Para->coor_originValue[i]);
 	for(i=0;i<3;i++)
 		printf("NC_Para->coor_destinationValue[%d]:%ld \r\n",i,pNC_Para->coor_destinationValue[i]);
		
		printf("NC_Para->f:%ld \r\n",pNC_Para->f);
		printf("NC_Para->G_instruct:%dl \r\n",pNC_Para->G_instruct);
		printf("NC_Para->IS_INTERPOLATINO:%dl \r\n",pNC_Para->IS_INTERPOLATINO);
		
		printf("NC_Para->O_R:%ld \r\n",pNC_Para->O_R);
		printf("NC_Para->O_X:%f\r\n",pNC_Para->O_X);
		printf("NC_Para->O_Y:%f \r\n",pNC_Para->O_Y);
		printf("NC_Para->steps:%ld\r\n",pNC_Para->steps);
		
}

//G代码数据显示到串口
//只在程序调试时候使用，因为输出这么多串口内容 会占用太长的CPU时间
void printGCode_Msg(GCode_Msg * pGCode_Msg)
{
		int i=0;

 	printf("print Gcode_Msg----------\r\n");
	for(i=0;i<3;i++)
		printf("pGCode_Msg->coor_value[%d]: %f \r\n",i,pGCode_Msg->coor_value[i]);
	printf("pGCode_Msg->O_R: %f \r\n",pGCode_Msg->O_R);
	
	printf("pGCode_Msg->G_instruct: %d \r\n",pGCode_Msg->G_instruct);
	printf("pGCode_Msg->F: %d \r\n",pGCode_Msg->F);
	printf("pGCode_Msg->M_instruct: %d \r\n",pGCode_Msg->M_instruct);
	
	
}


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







