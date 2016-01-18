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




//�Ƿ�ͨ���������һЩ������ִ����Ϣ   1����  0����
#define USE_MAIN_DEBUG (0)

u16 POINT_COLOR_LAST;//��ʱ��Ż�����ɫ
u8 ZOOM=1;//�Ѷ�ȡ����G���������ߴ�Ŵ�ZOOM�������ڵ���
u16 Temp_X,Temp_Y; //��������ƽ��

GCode_Msg * pGCode_Msg;
NC_Para * pNC_Para;
char* pcode;	//ȡһ�е�ָ��


//�������´���,֧��UCOS,������Ҫѡ��use MicroLIB	 
_ttywrch(int ch)
{
ch = ch;
}

//��ʼ����ʾ���ݣ����ݸ�ʽΪ5λ������
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
//ѡ���ļ�
//path:·��
//offset �׸��ļ�����ƫ��offset��
//����ֵ:ִ�н��
FRESULT mf_select_file(char * filename,char * path,char offset)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
	//FILINFO fileinfoTemp;	//ͷ�ļ���Ϣ��ʱ����
#if S_MAX_SIZ
	fileinfo.fsize = S_MAX_SIZ * 2 + 1;
	fileinfo.fname ;
#endif		  
	
    res = f_opendir(&dir,(const char*)path); //��һ��Ŀ¼

// 	if (f_readdir(&dir, &fileinfoTemp)!=FR_OK)                 //��ȡĿ¼�µ�һ���ļ�)
// 	{
// 		printf("\n�ļ�����\n");
// 	}
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(offset--)
		{
			res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
			if (res != FR_OK || fileinfo.fname[0] == 0) 
			{ 
				res = f_opendir(&dir,(const char*)path); //��һ��Ŀ¼
				if (res == FR_OK) offset++;continue;
				//printf("�ļ�ĩβ");
				//break; 
			}//������/��ĩβ��,�˳�
			
			//if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
#if S_MAX_SIZ
			fn = *fileinfo.fname ? fileinfo.fname : fileinfo.fname;
#else							   
			fn = fileinfo.fname;
#endif	                                              /* It is a file. */

		} 
		//printf("%s/", path);//��ӡ·��	
		//	printf("%s\n",  fn);//��ӡ�ļ���	
				if(*fn!='\0')
		{
				printf("%s\n",  fn);//��ӡ�ļ���
			strcpy(filename,fn);//����ļ�
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
//�õ�Ŀ¼�׵�ַ����ƫ��λ�õ��ļ���
//path:·��
//offset �׸��ļ�����ƫ��offset��
//����ֵ:ִ�н��
FRESULT mf_get_fileName(char * filename,char * path,char offset)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
	//FILINFO fileinfoTemp;	//ͷ�ļ���Ϣ��ʱ����
#if S_MAX_SIZ
	fileinfo.fsize = S_MAX_SIZ * 2 + 1;
	fileinfo.fname ;
#endif		  
	
    res = f_opendir(&dir,(const char*)path); //��һ��Ŀ¼
	
	// 	if (f_readdir(&dir, &fileinfoTemp)!=FR_OK)                 //��ȡĿ¼�µ�һ���ļ�)
	// 	{
	// 		printf("\n�ļ�����\n");
	// 	}
    if (res == FR_OK) 
	{	
		//printf("\r\n"); 
		while(offset--)
		{
			res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
			if (res != FR_OK || fileinfo.fname[0] == 0) //û����ȷ��ȡ���ļ���������ߵ��˽�β
			{ 
				//res = f_opendir(&dir,(const char*)path); //���˽�β���ٴδ����Ŀ¼��ʼλ��
				//if (res == FR_OK) offset++;continue;//�������½���һ�ζ�ȡ
				if (res != FR_OK )
				{
				//	printf("\n��ȡ����\n");
				}
				else
				{
				//	printf("\n�ļ�ĩβ\n");
				}
				break; 
			}//������/��ĩβ��
			
			//if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
#if S_MAX_SIZ
			fn = *fileinfo.fname ? fileinfo.fname : fileinfo.fname;
#else							   
			fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			
		} 
	
		if(*fn!='\0')
		{
		//	printf("%s\n",  fn);//��ӡ�ļ���
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
�ӹ��켣UI
*/
void LCD_ShowFrame(void){
	//���ô�����ɫ
	LCD_Clear(BLACK);
	LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//���ָ����ɫ
	LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//���ָ����ɫ
	
	POINT_COLOR_LAST=POINT_COLOR;//���滭����ɫ
	POINT_COLOR=BLACK;//�л�������ɫ
	//���
	LCD_DrawRectangle(INTERVAL/5, INTERVAL/5, lcddev.width-1-INTERVAL/5, lcddev.height-1-INTERVAL/5);
	//�ϱ߿�
	LCD_DrawRectangle(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL);
	//�±߿�
	LCD_DrawRectangle(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL);
	
	//��������
	LCD_DrawLine(INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2, lcddev.width/2);
	//��X���ͷ
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2-INTERVAL*0.6);
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2+INTERVAL*0.6);
	//��������
	LCD_DrawLine(lcddev.width/2, INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2);
	//Y���ͷ
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2-INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2+INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	
	POINT_COLOR=POINT_COLOR_LAST;//�ָ�������ɫ
	
	//�ı���Ϣ
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

////ѡ������Ŵ���
void NC_setZoom(void)
{
	char key;
	key=KEY_Scan(0);
	LCD_ShowString(Temp_X, Temp_Y,8*60,16,16,"PRES WK ZOOM Pra");
	LCD_ShowString(Temp_X, Temp_Y+16,8*60,16,16,"key1 for confirm");
		while(key!=KEY1_PRES)//����ȷ�ϼ�
		{
			if (key==WKUP_PRES)
			{
				LCD_ShowString(Temp_X, Temp_Y,8*20,16,16,"                   ");
				LCD_ShowString(Temp_X, Temp_Y+16,8*20,16,16,"                   ");
				ZOOM=(ZOOM<<1);//�Ŵ�������2
				if(ZOOM==0) ZOOM=1;//��������� �����´�1��ʼ
				LCD_ShowString(Temp_X, Temp_Y,8*9,16,16,"ZOOM:");
				LCD_ShowNum(Temp_X+8*11, Temp_Y,ZOOM,4,16); 
			}
			
			//key=getchar();
			key=KEY_NULL;
			key=KEY_Scan(0);
	}
}

////////////////////////////////////////////////////////////////////////////
////���ã����ļ�ϵͳ��ѡ���ļ�
////�����ѡ�����ļ�������,FATFSûʹ�ó��ļ���ʽ���ļ���ӦС�ڰ˸��ַ�
////������s������ѡ��f������ȷ��
////////////////////////////////////////////////////////////////////////////
FRESULT select_file(char *filenameTemp){
	FRESULT res;
	char key;
	char select_num=0;//ѡȡ���ļ���Ŀ¼�е�λ��
	//key=getchar();
	key=KEY_Scan(0);
	while(key!=KEY1_PRES){
		//��ĸ��s������ѡ�� 
		if (key==KEY0_PRES)
		{
			select_num++;
			//�л��ļ�
			res=mf_select_file(filenameTemp,("0:/NCCODE"),select_num);
			if((res!=FR_OK)||(*filenameTemp=='\0')) {return FR_INVALID_NAME;}//ѡ�����
			
			//�ı���Ϣ��ʾ��LCD
		Temp_X=INTERVAL+3;
		Temp_Y=Temp_Y=lcddev.width+3;
			BACK_COLOR=WHITE;
			LCD_ShowString(Temp_X, Temp_Y+16*1,8*30,16,16,(u8 *)"                           ");//��ո��е���ʾ
			LCD_ShowString(Temp_X, Temp_Y+16*1,8*9,16,16,(u8 *)filenameTemp);
		 	
		}
		
		//key=getchar();
		key=KEY_NULL;
		key=KEY_Scan(0);
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////
////���ã����ļ�ϵͳ��ѡ���ļ�
////�����ѡ�����ļ�������,FATFSûʹ�ó��ļ���ʽ���ļ���ӦС�ڰ˸��ַ�
////���أ��������
////////////////////////////////////////////////////////////////////////////
FRESULT get_fileName(char *filenameTemp){
	FRESULT res;

	static char select_num=1;//ѡȡ���ļ���Ŀ¼�е�λ��

	res=mf_get_fileName(filenameTemp,"0:/NCCODE",select_num++);
	if((res!=FR_OK)||(*filenameTemp=='\0')) {return FR_INVALID_NAME;}//ѡ�����

	return res;
}


/*
ѡ�ļ���UI
��һ��������ѡ����ļ����� �ı����ַ
*/
void selectFileFromSD(char * filename)
{
		char numTemp=0;	//ѡ�񵽵��ļ���Ŀ¼�е�λ��
		char name[20]; //��ȡ���ļ����ı�������
		//	
		//����
		LCD_Clear(BLACK);
		LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//���ָ����ɫ
		LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//���ָ����ɫ
			//�ı���Ϣ
		Temp_X=INTERVAL+3;   
		Temp_Y=INTERVAL+3;	 
		//------
		
		memset(name,'\0',sizeof(name));	//	������NAME���Ϊ0
		
	//	//ѡ�����ݷŴ���
	//	NC_setZoom( );//�Ŵ��
		
		BACK_COLOR=YELLOW;//��ɫ����
		LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,"FILE DIRCTORY:-----------");
		
		while (get_fileName(name)==FR_OK)  // get_fileName��ȡ�ļ��������������ļ���
		{
			numTemp++;				 //	  ͳ���ļ�����
			printf("F%-2d:%s \r\n",numTemp,name);
			LCD_ShowString(Temp_X, Temp_Y+16*numTemp,8*9,16,16,(u8 *)name);	
		}
		LCD_ShowString(Temp_X, Temp_Y+16*(numTemp+1),8*30,16,16,"-------------------------");
		memset(name,'\0',sizeof(name));
		
		//��ʾ��ʾ��Ϣ
		//�ı���Ϣ��ʾ��LCD
		Temp_X=INTERVAL+3;
		Temp_Y=Temp_Y=lcddev.width+3;
		BACK_COLOR=WHITE;
		POINT_COLOR=BLACK;
		LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,(u8 *)"THE CHOOSED FILE:---------");
		LCD_ShowString(Temp_X, Temp_Y+16*1,8*30,16,16,(u8 *)"Pre Key0 to choose file! ");
		LCD_ShowString(Temp_X, Temp_Y+16*2,8*30,16,16,(u8 *)"(KEY0 for changing file; ");
		LCD_ShowString(Temp_X, Temp_Y+16*3,8*30,16,16,(u8 *)" KEY1 for confirming .)");
	
		while (*name==0)//��ǰ�ļ���Ϊ��
		{
			select_file(name);	 //ѡ�ļ�
			if (*name==0) printf("\nδ��ȷѡ���ļ�\r\n");
		}
	
		printf("choose----->:%s\n",name);
		//�����ļ���
		memcpy(filename,name,strlen(name)+1);

}

//ͨ���ļ�����ȡ�ļ�����
//��һ������������ļ�
//�ڶ����������ļ�����Ĭ����SD��:/NCCODEĿ¼�£�
FRESULT getFile(FIL * outFile,char *filename)
{
		FRESULT res;
		char *default_dir="0:/NCCODE";	//Ĭ�ϵĳ�ʼ�ļ�Ŀ¼
		char dirtemp[20];//Ŀ¼��ʱ���
	
		POINT_COLOR=RED;
		LCD_ShowFrame();//���ƿ��
		updataShow(pNC_Para,0);//��ʾ��������
	
	printf("%s----------",dirtemp);	//����ʱ��ʾ�ڴ���
	strcpy(dirtemp,default_dir);
	strcat(dirtemp,"/");//�õ�����·������
	strcat(dirtemp,filename);
	printf("%s----------",dirtemp);
	res = f_open(outFile, dirtemp, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);//��ȡ�ľ���Ŀ¼�ļ���m_file
	memset(dirtemp,'\0',sizeof(dirtemp));//�ͷ�����
  if(res != 0)
        printf("Open File(%s) failed.\r\n",filename);
  else
        printf("open Flie(%s) successed.\r\n",filename); 
	return res;
}

//ִ�д�����ļ�
void nc_excuteFile(FIL *m_file )
{
		str=mymalloc(SRAMIN,96);
		pcode=mymalloc(SRAMIN,96);//��̬����ռ�
		printf("\r\n-----------------EXpalianation  Begined!!!----------------------\r\n\r\n");		
		
		pGCode_Msg=(GCode_Msg*)malloc(sizeof(GCode_Msg));
		pNC_Para=(NC_Para*)malloc(sizeof(NC_Para));
		
		while(((pcode=getALineCode(m_file))!=NULL))
		{ 
#if USE_MAIN_DEBUG
			printf("��ȡ����:%s\r\n",pcode);

			//1����
			printf("����\r\n");
#endif
			decode(pcode);
			//printGCode_Msg(pGCode_Msg);
		
			
			//2�ȴ�������
#if USE_MAIN_DEBUG
			printf("��ʼ�ȴ�\r\n");
#endif
			while(pNC_Para->IS_INTERPOLATINO==1) LED1=0;//��ʱ������������������
			LED1=1;//�岹״̬�ƹر�
			
				//3װ�ز���
#if USE_MAIN_DEBUG
			printf("װ��\r\n");
#endif
			prepare_interpolation(pGCode_Msg,pNC_Para);//���浽��һ���ṹ��
			
			//updataShow(pNC_Para,0);
			//��ʾ������
			
			//printNCPara(pNC_Para);
			
			//4����������
#if USE_MAIN_DEBUG
			printf("��������\r\n");
#endif
			interpolationChangeSpeed(1);//10ms
			
			//��ʾ��LCD,ʹ����ʾ������ʱ��
			//��ʾģʽ0��ȫ���岹����ˢ����ʾ
			updataShow(pNC_Para,0);
			
			interpolationTaskCreat();
		}
			
		printf("\r\n\r\n-----------------EXpalianation  Finished!!!----------------------\r\n");
		
		myfree(SRAMIN,pcode);  //�ͷ�pcode�Ŀռ�
                  
    f_close(m_file);
		
  //LCD_PutString(0,64, "close successed."); 
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
			BACK_COLOR=YELLOW;
		LCD_ShowString(2*INTERVAL,2*INTERVAL,200,12,12,"CODE FINISH!");
	
} 

//�岹������ʾ������
//ֻ�ڳ������ʱ��ʹ�ã���Ϊ�����ô�മ������ ��ռ��̫����CPUʱ��
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

//G����������ʾ������
//ֻ�ڳ������ʱ��ʹ�ã���Ϊ�����ô�മ������ ��ռ��̫����CPUʱ��
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


// //����
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







