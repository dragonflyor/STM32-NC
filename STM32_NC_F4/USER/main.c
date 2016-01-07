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

 
#define INTERVAL 5  //LCD��ͼ�ĵ�λ

//�Ƿ�ͨ���������һЩ������ִ����Ϣ   1����  0����
#define USE_MAIN_DEBUG (0)

u16 POINT_COLOR_LAST;//��ʱ��Ż�����ɫ
//u16 BACK_COLOR_LAST;//��ʱ��ű�����ɫ

FATFS m_fs;	//�ļ�ϵͳ����
FRESULT res;   //�ļ������Ľ������ֵ
FIL m_file;	 //�ļ�����
GCode_Msg * pGCode_Msg;
NC_Para * pNC_Para;
char* pcode;	//ȡһ�е�ָ��
extern u16 Temp_X,Temp_Y; //��������ƽ��

u8 ZOOM=1;//�Ѷ�ȡ����G���������ߴ�Ŵ�ZOOM�������ڵ���

void LCD_ShowFrame(void);  //lCD��ʾ���ڣ���ʼ��
FRESULT get_fileName(char *filenameTemp);  //ͨ������ѡ���ı�����
FRESULT select_file(char *filenameTemp); //ѡ���ļ�


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


int main(void)
{
	u8 t=0;
	
	 
	char numTemp=0;	//?
	char name[20]; //��ȡ���ļ����ı�������
	char *dir="0:/NCCODE";	//Ĭ�ϵĳ�ʼ�ļ�Ŀ¼
	char dirtemp[20];//Ŀ¼��ʱ���
	 
	

//ADD by XZ 20160107
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
// ADDEND

	
	str=mymalloc(SRAMIN,96);
//	 
//	NVIC_Configuration();  //�ж�����	 
	//M3!=M4
	delay_init(168);	    	 //��ʱ������ʼ��	  
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	exfuns_init();		//Ϊfatfs��ر��������ڴ�				 
	LCD_Init();			//��ʼ��Һ�� 
	LED_Init();         //LED��ʼ��	 
	KEY_Init();		    //�����ĳ�ʼ��
	//M4+
	my_mem_init(SRAMIN);

// 	mem_init();			//��ʼ���ڴ��	
//	
//	driver_GPIOInit1( );
//	
//	//axisToZero1( );//����
//	
	LCD_ShowString(10,90,200,24,24,     "  Welcome to the");
 	LCD_ShowString(10,90+24*1,200,24,24,"  embeded system!");	 
	LCD_ShowString(10,90+24*2,200,16,16,"   Thanks for suporting!");	
//	
	delay_ms(1000);
	//while(SD_Initialize())					//���SD��
	//M4 modify
	while(SD_Init())
	{
		LCD_ShowString(60,150,200,16,16,"SD Card Error!");	//���û��⵽����ʾ����
		printf("%X:SD Card Error!\r\n",SD_Init()); //��ʾ���������
		delay_ms(200);
		LCD_Fill(60,150,240,150+16,WHITE);//�����ʾ			  
		delay_ms(200);
		LED0=!LED0;//DS0��˸
	}	
	
	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  f_mount(fs[0],"0:",1); 			//����SD�� 
	

	
	
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
	

	
	
	POINT_COLOR=RED;
	LCD_ShowFrame();//���ƿ��
	//��ʾ������ֵ
//	LCD_UpdataShow(x,y,  	current_position[0],current_position[1],
//							destination[0],destination[1],
//								0,0);
	updataShow(pNC_Para,0);//��ʾ��������
	
	
	
	
	printf("%s----------",dirtemp);	//����ʱ��ʾ�ڴ���
	strcpy(dirtemp,dir);
	strcat(dirtemp,"/");//�õ�����·������
	strcat(dirtemp,name);
	printf("%s----------",dirtemp);

	res = f_open(&m_file, dirtemp, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);//��ȡ�ľ���Ŀ¼�ļ���m_file
	memset(dirtemp,'\0',sizeof(dirtemp));//�ͷ�����
  if(res != 0)
        printf("Open File(%s) failed.\r\n",name);
    else
        printf("open Flie(%s) successed.\r\n",name); 
         
		
	pcode=mymalloc(SRAMIN,96);//��̬����ռ�
	printf("\r\n-----------------EXpalianation  Begined!!!----------------------\r\n\r\n");		
		
		pGCode_Msg=(GCode_Msg*)malloc(sizeof(GCode_Msg));
		pNC_Para=(NC_Para*)malloc(sizeof(NC_Para));
		
		
		
		
		while(((pcode=getALineCode(&m_file))!=NULL))
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
                  
    f_close(&m_file);
		
  //LCD_PutString(0,64, "close successed."); 
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
			BACK_COLOR=YELLOW;
		LCD_ShowString(2*INTERVAL,2*INTERVAL,200,12,12,"CODE FINISH!");
	
	while(1)
	{
		t++;
		delay_ms(200);		 			   
		LED0=!LED0;
	} 

}

