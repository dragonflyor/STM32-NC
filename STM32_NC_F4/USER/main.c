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

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	//M3!=M4
	delay_init(168);	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
 	exfuns_init();		//Ϊfatfs��ر��������ڴ�				 
	LCD_Init();			//��ʼ��Һ�� 
	LED_Init();         //LED��ʼ��	 
	KEY_Init();		    //�����ĳ�ʼ��
	//M4+
	my_mem_init(SRAMIN);
	


//	driver_GPIOInit1( );
//	axisToZero1( );//����

	LCD_ShowString(10,90,200,24,24,     "  Welcome to the");
 	LCD_ShowString(10,90+24*1,200,24,24,"  embeded system!");	 
	LCD_ShowString(10,90+24*2,200,16,16,"   Thanks for suporting!");	

	delay_ms(1000);
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
	
	
	fliename =mymalloc(SRAMIN,20);//��̬����ռ�
	memset(fliename,'\0',strlen(fliename));
	//��UI���ļ�ѡ��
	selectFileFromSD(fliename);
	
	//��ȡ���ļ���
	printf(">>>>filemane:%s \r\n",fliename);
	
	//��ȡ�ļ�����
	getFile(&readFile,fliename);
	myfree(SRAMIN,fliename);

	
	//��UI��ִ���ļ�
	nc_excuteFile(&readFile);
	
	while(1)
	{
		t++;
		delay_ms(200);		 			   
		LED0=!LED0;
	} 

}

