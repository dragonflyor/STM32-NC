
#include "APPS.h"

/************************0.��������*start_task*************************/
//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

/************************1.����1*task1_task*************************/
//�������ȼ�
#define TASK1_TASK_PRIO		4
//�����ջ��С	
#define TASK1_STK_SIZE 		128
//������ƿ�
OS_TCB Task1_TaskTCB;
//�����ջ	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

/************************2.����2*task2_task*************************/
//�������ȼ�
#define TASK2_TASK_PRIO		5
//�����ջ��С	
#define TASK2_STK_SIZE 		128
//������ƿ�
OS_TCB Task2_TaskTCB;
//�����ջ	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//������
void task2_task(void *p_arg);


/************************3.��������*com_task*************************/
//�������ȼ�
#define COM_TASK_PRIO		6
//�����ջ��С	
#define COM_STK_SIZE 		512
//������ƿ�
OS_TCB COM_TaskTCB;
//�����ջ	
CPU_STK COM_TASK_STK[COM_STK_SIZE];
//������
void com_task(void *p_arg);


//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };


//��ʼ����������
void start_task(void *p_arg)
{
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;

		CPU_Init();
	#if OS_CFG_STAT_TASK_EN > 0u
		 OSStatTaskCPUUsageInit(&err);  	//ͳ������                
	#endif
		
	#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
			CPU_IntDisMeasMaxCurReset();	
	#endif
		
	#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
		 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
		OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
	#endif	
		
		OS_CRITICAL_ENTER();	//�����ٽ���
		//����TASK1����
		OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
					 (CPU_CHAR	* )"Task1 task", 		
									 (OS_TASK_PTR )task1_task, 			
									 (void		* )0,					
									 (OS_PRIO	  )TASK1_TASK_PRIO,     
									 (CPU_STK   * )&TASK1_TASK_STK[0],	
									 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
									 (CPU_STK_SIZE)TASK1_STK_SIZE,		
									 (OS_MSG_QTY  )0,					
									 (OS_TICK	  )0,					
									 (void   	* )0,					
									 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
									 (OS_ERR 	* )&err);				
					 
		//����TASK2����
		OSTaskCreate((OS_TCB 	* )&Task2_TaskTCB,		
					 (CPU_CHAR	* )"task2 task", 		
									 (OS_TASK_PTR )task2_task, 			
									 (void		* )0,					
									 (OS_PRIO	  )TASK2_TASK_PRIO,     	
									 (CPU_STK   * )&TASK2_TASK_STK[0],	
									 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
									 (CPU_STK_SIZE)TASK2_STK_SIZE,		
									 (OS_MSG_QTY  )0,					
									 (OS_TICK	  )0,					
									 (void   	* )0,				
									 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
									 (OS_ERR 	* )&err);			 
			
/*
								 //�������ȼ�
#define COM_TASK_PRIO		6
//�����ջ��С	
#define COM_STK_SIZE 		128
//������ƿ�
OS_TCB COM_TaskTCB;
//�����ջ	
CPU_STK COM_TASK_STK[COM_STK_SIZE];
//������
void com_task(void *p_arg);
								 */								 
	//������������
		OSTaskCreate((OS_TCB 	* )&COM_TaskTCB,		
				 (CPU_CHAR	* )"com task", 		
                 (OS_TASK_PTR )com_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )COM_TASK_PRIO,     	
                 (CPU_STK   * )&COM_TASK_STK[0],	
                 (CPU_STK_SIZE)COM_STK_SIZE/10,	
                 (CPU_STK_SIZE)COM_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			 
	
								 
		OS_CRITICAL_EXIT();	//�˳��ٽ���
									 
		OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������							 
}


//task1������
void task1_task(void *p_arg)
{
		u8 task1_num=0;
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;
		
		POINT_COLOR = BLACK;
		OS_CRITICAL_ENTER();
		LCD_DrawRectangle(5,110,115,314); 	//��һ������	
		LCD_DrawLine(5,130,115,130);		//����
		POINT_COLOR = BLUE;
		LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
		OS_CRITICAL_EXIT();
		while(1)
		{
				task1_num++;	//����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
				LED0= ~LED0;
				printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
				if(task1_num==5) 
				{
						OSTaskDel((OS_TCB*)&Task2_TaskTCB,&err);	//����1ִ��5�˺�ɾ��������2
						printf("����1ɾ��������2!\r\n");
				}
				LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //�������
				LCD_ShowxNum(86,111,task1_num,3,16,0x80);	//��ʾ����ִ�д���
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
			
		}
}

//task2������
void task2_task(void *p_arg)
{
		u8 task2_num=0;
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;
		
		POINT_COLOR = BLACK;
		OS_CRITICAL_ENTER();
		LCD_DrawRectangle(125,110,234,314); //��һ������	
		LCD_DrawLine(125,130,234,130);		//����
		POINT_COLOR = BLUE;
		LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
		OS_CRITICAL_EXIT();
		while(1)
		{
				task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
				LED1=~LED1;
				printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
				LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //��ʾ����ִ�д���
				LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //�������
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
		}
}



//����������
void com_task(void *p_arg)
{
		/*
					//void mymemset(void *s,u8 c,u32 count);	//�����ڴ�
					//void mymemcpy(void *des,void *src,u32 n);//�����ڴ�  
		
		*/
		u8 com_num=0;
		OS_ERR err;
		CPU_SR_ALLOC();
		p_arg = p_arg;
		
		POINT_COLOR = BLACK;
		OS_CRITICAL_ENTER();
		
		LCD_DrawRectangle(125+120,110,234+120,314); //��һ������	
		LCD_DrawLine(125+120,130,234+120,130);		//����
		POINT_COLOR = GRAY;
		LCD_ShowString(126+120,111,110,16,16,"com Run:000");
		
		
		LCD_DrawRectangle(1,lcddev.height-40,lcddev.width,lcddev.height-1); //��Ļ�ײ���һ������	
		LCD_DrawLine(1,lcddev.height-20,lcddev.width,lcddev.height-20);		//����
		LCD_ShowString(2,lcddev.height-39,500,16,16,"Received data from srialport:"); //����
		
		OS_CRITICAL_EXIT();
		while(1)
		{
				com_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
				LED1=~LED1;
				printf("���������Ѿ�ִ�У�%d��\r\n",com_num);
				LCD_ShowxNum(206+120,111,com_num,3,16,0x80);  //��ʾ����ִ�д���
				LCD_Fill(126+120,131,233+120,313,lcd_discolor[13-com_num%14]); //�������
				//״̬��β���ң���ɫ��˸��ʾϵͳ��������������ѯ����
				LCD_Fill(lcddev.width-19,lcddev.height-39,lcddev.width,lcddev.height-21,lcd_discolor[13-com_num%2]); //���״̬�ź�
	
				//-----------------------------------------------------------------------
				//�ɹ����յ�����
				if(USART_RX_STA&0x8000)
				{
						//���յ������ݳ���
						int len=USART_RX_STA&0x3fff;
						char * conn_recv;

						//�����ڴ�
						conn_recv = (char*)mymalloc(SRAMIN,len+1);
						mymemset(conn_recv,0,len+1);	//�����ڴ�
						
						//ȡ���ڵ����� 
						mymemcpy(conn_recv, USART_RX_BUF,len);
						
						//״̬��λ
						USART_RX_STA=0;
						
						//��ʾ����״̬����Ļ
						com_statusBar(conn_recv);
						
						if(conn_recv[0]=='>' && conn_recv[1]=='>')
						{
								printf("���ļ�\r\n");
								m_creatFile("0:/NCCODE/conndata.txt",10,200);
								printf("�������ļ�\r\n");
						}

						//�ͷ��ڴ�
						myfree(SRAMIN,conn_recv); 	
				}
				//-----------------------------------------------------------------------
				OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
			
		}
}



