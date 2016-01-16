#include "board.h"

	//APP����ɫ
	extern int lcd_discolor[14];

//Ӳ����ʼ��
void borad_HarwareInit(void)
{
		//OS_ERR err;
		//CPU_SR_ALLOC();
		
		delay_init(168);  //ʱ�ӳ�ʼ��
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
		uart_init(115200);   //���ڳ�ʼ��
		
		INTX_DISABLE();		//���ж�,��ֹ�δ�ʱ���������ʼ���Ĵ���
		
		LED_Init();         //LED��ʼ��	
		LCD_Init();			//LCD��ʼ��	
		
		POINT_COLOR = RED;
		LCD_ShowString(30,10,200,16,16,"HardWare: STM32F4");	
		LCD_ShowString(30,30,200,16,16,"SotfWareSystem:UCOSIII,FATFS");
		LCD_ShowString(30,50,200,16,16,"STM32_NC");
		LCD_ShowString(30,70,200,16,16,"HuBei University of Technology");
		LCD_ShowString(30,90,200,16,16,"2016/1/16");
		
		//SD����ʼ��
		SDCard_Init();
		
		INTX_ENABLE();		//���ж�
}

void SDCard_Init(void)
{
		u32 total,free;
		u8 res=0;	
		
		W25QXX_Init();				//��ʼ��W25Q128
		my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
		my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
		
		while(SD_Init())//��ⲻ��SD��
		{
				LCD_ShowString(30,150,200,16,16,"SD Card Error!");
				delay_ms(500);					
				LCD_ShowString(30,150,200,16,16,"Please Check! ");
				delay_ms(500);
				LED0=!LED0;//DS0��˸
		}
		exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
		f_mount(fs[0],"0:",1); 					//����SD�� 
		res=f_mount(fs[1],"1:",1); 				//����FLASH.	
		if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
		{
				LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//��ʽ��FLASH
				res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
				if(res==0)
				{
						f_setlabel((const TCHAR *)"1:ALIENTEK");	//����Flash���̵�����Ϊ��ALIENTEK
						LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//��ʽ�����
				}else LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//��ʽ��ʧ��
				delay_ms(1000);
		}													    
		LCD_Fill(30,150,240,150+16,WHITE);		//�����ʾ			  
		while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
		{
				LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
				delay_ms(200);
				LCD_Fill(30,150,240,150+16,WHITE);	//�����ʾ			  
				delay_ms(200);
				LED0=!LED0;//DS0��˸
		}													  			    
		POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
		LCD_ShowString(30,150,200,16,16,"FATFS OK!");	 
		LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");	 
		LCD_ShowString(30,190,200,16,16,"SD  Free Size:     MB"); 	    
		LCD_ShowNum(30+8*14,170,total>>10,5,16);				//��ʾSD�������� MB
		LCD_ShowNum(30+8*14,190,free>>10,5,16);					//��ʾSD��ʣ������ MB		

}


//���ڽ������ݱ������ļ���
//��ѯtimeout�λ�û���ݶ�����˳�
//cycletime ���ļ�ʱ��ѯɨ������� ��λms
void m_creatFile(char * pathname ,int timeout,u16 cycletime)
{
		u8 t=0;	
		mf_open((u8*)pathname,FA_OPEN_ALWAYS| FA_READ|FA_WRITE);
	
		while(1)
		{
				/*
				---------------------����״̬-------------------------
						//����1�жϷ������
						//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
						//u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
						//����״̬
						//bit15��	������ɱ�־
						//bit14��	���յ�0x0d
						//bit13~0��	���յ�����Ч�ֽ���Ŀ
						//u16 USART_RX_STA=0;       //����״̬���	
				---------------------FATFS����-------------------------
						//void mymemset(void *s,u8 c,u32 count);	//�����ڴ�
						//void mymemcpy(void *des,void *src,u32 n);//�����ڴ�
						//void myfree(u8 memx,void *ptr);  			//�ڴ��ͷ�(�ⲿ����)		
						//u8 mf_write(u8*dat,u16 len);

				*/
				
				//�ɹ�����һ������
				if(USART_RX_STA&0x8000)
				{

						//�����յ����ݳ���
						int len = USART_RX_STA&0x3fff; 
					
						//���ٿռ�
						char * conn_recv = (char*)mymalloc(SRAMIN,len+1);
						mymemset(conn_recv,0,len+1);	//�����ڴ�
						
						//ȡ���ڵ����� 
						mymemcpy(conn_recv, USART_RX_BUF,len);
				


						//��ʾ����Ļ
						com_statusBar(conn_recv);
					
						//����$$��ʼ���ַ�����ʾ�ļ��������
						if(conn_recv[0]=='$'&&conn_recv[1]=='$')
						{
								//�ر��ļ�
								mf_close();
								//�ͷ��ڴ�
								myfree(SRAMIN,conn_recv); 
								//״̬��λ
								USART_RX_STA=0;
								//break;
								return ;
						}
						//״̬��λ
						USART_RX_STA=0;
					
						//���ڽ��յ������ݱ��浽����
						mf_write((u8*)conn_recv,len);
						//д���з�\n
						mf_write((u8*)"\n",1);
						
						//�ͷ��ڴ�
						myfree(SRAMIN,conn_recv); 
						
						t=0;
				}
			
				t++; 
				//��ʱ�˳�
				if(t == timeout){
					return;
				}
				//������ѯʱ�������200ms
				delay_ms(cycletime);		 			   
				LED0=!LED0;
		} 
}

//��ʾ�����յ�������
//comdata:Ҫ��ʾ������
void com_statusBar(char * comdata){
	static u8 rec_count = 0;
	LCD_DrawRectangle(1,lcddev.height-40,lcddev.width-1,lcddev.height-1); //��Ļ�ײ���һ������	
	LCD_DrawLine(1,lcddev.height-20,lcddev.width,lcddev.height-20);		//����
	LCD_ShowString(2,lcddev.height-39,500,16,16,"Received data from srialport:"); //����
	
	LCD_Fill(2,lcddev.height-19,lcddev.width-2,lcddev.height-2,WHITE);
	LCD_ShowString(2,lcddev.height-19,500,16,16,(u8*)comdata); //����
	
	//״̬��β������ɫ��˸����ʾϵͳ�����������ڽ�������
	LCD_Fill(lcddev.width-19,lcddev.height-39,lcddev.width,lcddev.height-21,lcd_discolor[10-rec_count%2]); //���״̬�ź�
}

