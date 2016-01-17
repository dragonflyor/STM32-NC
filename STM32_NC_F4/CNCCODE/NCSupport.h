/*
		该文件是为了实现NC 而自己包装的方法
*/

#ifndef __NCSUPPORT_H
#define __NCSUPPORT_H
	#include "lcd.h"
	#include "fattester.h"	 
	#include "exfuns.h"
	#include "interpolation.h"
	#include "getAlineCodeFromSDtxt.h"
	
	
	#define KEY_NULL 0
	
	#define INTERVAL 5  //LCD绘图的单位
	
	extern u16 Temp_X,Temp_Y; //窗口坐标平移
	extern GCode_Msg * pGCode_Msg;
	extern NC_Para * pNC_Para;
	extern u16 POINT_COLOR_LAST;//临时存放画笔颜色
	extern u8 ZOOM;//把读取到的G代码的坐标尺寸放大ZOOM倍，便于调试
	//extern FATFS m_fs;	//文件系统变量
	//extern FRESULT res;   //文件操作的结果返回值
	//extern FIL m_file;	 //文件变量
	extern char* pcode;	//取一行的指针

	void LCD_UpdataShow( u32 x,u32 y,u32 x0,u32 y0,u32 xe,u32 ye,u32 g,u32 steps);
	FRESULT mf_select_file(char * filename,char * path,char offset);
	FRESULT mf_get_fileName(char * filename,char * path,char offset);
	
	void LCD_ShowFrame(void);  //lCD显示窗口，初始化
	FRESULT get_fileName(char *filenameTemp);  //通过按键选择文本名称
	FRESULT select_file(char *filenameTemp); //选择文件
	
	

	void selectFileFromSD(char * filename);
	FRESULT getFile(FIL * outFile,char *filename);
	void nc_excuteFile(FIL * file );
	
#endif
