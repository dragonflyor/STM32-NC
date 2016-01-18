#include "AppInterface.h"

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

#include "NCSupport.h"
#include "board.h"

//���ֶ�ѡ�ļ���ִ�е��������
void exec_manualSelectFile(void)
{
		char *fliename;
		FIL readFile;
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
}


//�����ļ�����ֱ��ִ��
void exec_byFilename(char *filename)
{
			FIL readFile;
	
		//��ȡ�ļ�����
		getFile(&readFile,filename);

		//��UI��ִ���ļ�
		nc_excuteFile(&readFile);

}


//�����ڴ�������Ϣ
void com_Process(void)
{
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
				
			//Э����">>01+filename":�����ļ�
				if(conn_recv[0]=='>' && conn_recv[1]=='>' && conn_recv[2]=='0' && conn_recv[3]=='1')
				{
					char * filename = (char *)mymalloc(SRAMIN,20);
					mymemset(filename,0,strlen(filename));
					mymemcpy(filename,&conn_recv[4],len-4);//�����ڴ�   
					printf("parsed filename : %s \r\n",filename);
					
						printf("���ļ�\r\n");
						//�����ļ�
					//��һ��������Ҫ������ļ�����Ĭ�ϱ����ַΪ0:/NCCODE/
						//�ڶ���������������ѯ��Σ�100�Σ�����û���ݽ��� ��������ļ���ʱ����ʱʱ��T=100*200ms
						//��������������ѯ����
						m_createFileDef(filename,100,200);
						printf("�������ļ�\r\n");
				}
				//Э����">>02+filename":�����ļ���ִ��
				else if(conn_recv[0]=='>' && conn_recv[1]=='>' && conn_recv[2]=='0' && conn_recv[3]=='2')
				{
					int key = 0;
					char * filename = (char *)mymalloc(SRAMIN,20);
					mymemset(filename,0,strlen(filename));
					mymemcpy(filename,&conn_recv[4],len-4);//�����ڴ�   
					printf("parsed filename : %s \r\n",filename);
					
					//1.�����ļ�
					m_createFileDef(filename,100,200);
						//OS_CRITICAL_ENTER();
					//2.ִ���ļ�
						exec_byFilename(filename);
						myfree(SRAMIN,filename);
						//�������ϵļ��򷵻ز˵�
						key = KEY_Scan(0);
						while(key != WKUP_PRES)
						{
							key = KEY_Scan(0);
						}
						//ϵͳ��λ
						SystemReset();
						//OS_CRITICAL_EXIT();
				}

				//�ͷ��ڴ�
				myfree(SRAMIN,conn_recv); 	
		}
		//-----------------------------------------------------------------------
}

