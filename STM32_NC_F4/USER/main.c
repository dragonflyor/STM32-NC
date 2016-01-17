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
//	char name[20]; //��ȡ���ļ����ı�������
//	char *dir="0:/NCCODE";	//Ĭ�ϵĳ�ʼ�ļ�Ŀ¼
//	char dirtemp[20];//Ŀ¼��ʱ���
	 
//ADD by XZ 20160107
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
// ADDEND

	

//	 
//	NVIC_Configuration();  //�ж�����	 
	//M3!=M4
	delay_init(168);	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
 	exfuns_init();		//Ϊfatfs��ر��������ڴ�				 
	LCD_Init();			//��ʼ��Һ�� 
	LED_Init();         //LED��ʼ��	 
	KEY_Init();		    //�����ĳ�ʼ��
	//M4+
	my_mem_init(SRAMIN);
	
	str=mymalloc(SRAMIN,96);

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
	
	printf("start \r\n");
	
//	
//	//����
//	LCD_Clear(BLACK);
//	LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//���ָ����ɫ
//	LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//���ָ����ɫ
//		//�ı���Ϣ
//	Temp_X=INTERVAL+3;   
//	Temp_Y=INTERVAL+3;	 
//	//------
//	
//	memset(name,'\0',sizeof(name));	//	������NAME���Ϊ0
//	
////	//ѡ�����ݷŴ���
////	NC_setZoom( );//�Ŵ��
//	
//	BACK_COLOR=YELLOW;//��ɫ����
//	LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,"FILE DIRCTORY:-----------");
//	
//		while (get_fileName(name)==FR_OK)  // get_fileName��ȡ�ļ��������������ļ���
//	{
//		numTemp++;				 //	  ͳ���ļ�����
//		printf("F%-2d:%s \r\n",numTemp,name);
//		LCD_ShowString(Temp_X, Temp_Y+16*numTemp,8*9,16,16,(u8 *)name);
//		
//	}
//	LCD_ShowString(Temp_X, Temp_Y+16*(numTemp+1),8*30,16,16,"-------------------------");

//	memset(name,'\0',sizeof(name));
//	
//		//��ʾ��ʾ��Ϣ
//		//�ı���Ϣ��ʾ��LCD
//		Temp_X=INTERVAL+3;
//		Temp_Y=Temp_Y=lcddev.width+3;
//		BACK_COLOR=WHITE;
//		POINT_COLOR=BLACK;
//		LCD_ShowString(Temp_X, Temp_Y,8*30,16,16,(u8 *)"THE CHOOSED FILE:---------");
//		LCD_ShowString(Temp_X, Temp_Y+16*1,8*30,16,16,(u8 *)"Pre Key0 to choose file! ");
//		LCD_ShowString(Temp_X, Temp_Y+16*2,8*30,16,16,(u8 *)"(KEY0 for changing file; ");
//		LCD_ShowString(Temp_X, Temp_Y+16*3,8*30,16,16,(u8 *)" KEY1 for confirming .)");
//	
//		while (*name==0)//��ǰ�ļ���Ϊ��
//	{
//	select_file(name);	 //ѡ�ļ�
//	if (*name==0) printf("\nδ��ȷѡ���ļ�\r\n");
//	}
//	
//printf("choose----->:%s\n",name);
//	

//	
//	
//	POINT_COLOR=RED;
//	LCD_ShowFrame();//���ƿ��
//	//��ʾ������ֵ
////	LCD_UpdataShow(x,y,  	current_position[0],current_position[1],
////							destination[0],destination[1],
////								0,0);
//	updataShow(pNC_Para,0);//��ʾ��������
//	
//	printf("%s----------",dirtemp);	//����ʱ��ʾ�ڴ���
//	strcpy(dirtemp,dir);
//	strcat(dirtemp,"/");//�õ�����·������
//	strcat(dirtemp,name);
//	printf("%s----------",dirtemp);

//	res = f_open(&m_file, dirtemp, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);//��ȡ�ľ���Ŀ¼�ļ���m_file
//	memset(dirtemp,'\0',sizeof(dirtemp));//�ͷ�����
//  if(res != 0)
//        printf("Open File(%s) failed.\r\n",name);
//  else
//        printf("open Flie(%s) successed.\r\n",name); 
//         
//	
		selectFileFromSD();
//	pcode=mymalloc(SRAMIN,96);//��̬����ռ�
//	printf("\r\n-----------------EXpalianation  Begined!!!----------------------\r\n\r\n");		
//		
//		pGCode_Msg=(GCode_Msg*)malloc(sizeof(GCode_Msg));
//		pNC_Para=(NC_Para*)malloc(sizeof(NC_Para));
//		
//		while(((pcode=getALineCode(&m_file))!=NULL))
//		{ 
//#if USE_MAIN_DEBUG
//			printf("��ȡ����:%s\r\n",pcode);

//			//1����
//			printf("����\r\n");
//#endif
//			decode(pcode);
//			//printGCode_Msg(pGCode_Msg);
//		
//			
//			//2�ȴ�������
//#if USE_MAIN_DEBUG
//			printf("��ʼ�ȴ�\r\n");
//#endif
//			while(pNC_Para->IS_INTERPOLATINO==1) LED1=0;//��ʱ������������������
//			LED1=1;//�岹״̬�ƹر�
//			
//				//3װ�ز���
//#if USE_MAIN_DEBUG
//			printf("װ��\r\n");
//#endif
//			prepare_interpolation(pGCode_Msg,pNC_Para);//���浽��һ���ṹ��
//			
//			//updataShow(pNC_Para,0);
//			//��ʾ������
//			
//			//printNCPara(pNC_Para);
//			
//			//4����������
//#if USE_MAIN_DEBUG
//			printf("��������\r\n");
//#endif
//			interpolationChangeSpeed(1);//10ms
//			
//			//��ʾ��LCD,ʹ����ʾ������ʱ��
//			//��ʾģʽ0��ȫ���岹����ˢ����ʾ
//			updataShow(pNC_Para,0);
//			
//			interpolationTaskCreat();
//		}
//			
//		printf("\r\n\r\n-----------------EXpalianation  Finished!!!----------------------\r\n");
//		
//		myfree(SRAMIN,pcode);  //�ͷ�pcode�Ŀռ�
//                  
//    f_close(&m_file);
//		
//  //LCD_PutString(0,64, "close successed."); 
// 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
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

