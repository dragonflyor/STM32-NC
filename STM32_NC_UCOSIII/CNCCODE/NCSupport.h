/*
		���ļ���Ϊ��ʵ��NC ���Լ���װ�ķ���
*/

#ifndef __NCSUPPORT_H
#define __NCSUPPORT_H
	#include "lcd.h"
	#include "fattester.h"	 
	#include "exfuns.h"
	#include "interpolation.h"
	#include "getAlineCodeFromSDtxt.h"
	
	
	#define KEY_NULL 0
	
	#define INTERVAL 5  //LCD��ͼ�ĵ�λ
	
	extern u16 Temp_X,Temp_Y; //��������ƽ��
	extern GCode_Msg * pGCode_Msg;
	extern NC_Para * pNC_Para;
	extern u16 POINT_COLOR_LAST;//��ʱ��Ż�����ɫ
	extern u8 ZOOM;//�Ѷ�ȡ����G���������ߴ�Ŵ�ZOOM�������ڵ���
	//extern FATFS m_fs;	//�ļ�ϵͳ����
	//extern FRESULT res;   //�ļ������Ľ������ֵ
	//extern FIL m_file;	 //�ļ�����
	extern char* pcode;	//ȡһ�е�ָ��

	void LCD_UpdataShow( u32 x,u32 y,u32 x0,u32 y0,u32 xe,u32 ye,u32 g,u32 steps);
	FRESULT mf_select_file(char * filename,char * path,char offset);
	FRESULT mf_get_fileName(char * filename,char * path,char offset);
	
	void LCD_ShowFrame(void);  //lCD��ʾ���ڣ���ʼ��
	FRESULT get_fileName(char *filenameTemp);  //ͨ������ѡ���ı�����
	FRESULT select_file(char *filenameTemp); //ѡ���ļ�
	
	

	void selectFileFromSD(char * filename);
	FRESULT getFile(FIL * outFile,char *filename);
	void nc_excuteFile(FIL * file );
	
#endif
