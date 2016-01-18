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
