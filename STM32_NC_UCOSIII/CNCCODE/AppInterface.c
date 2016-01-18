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

//被手动选文件并执行的任务调用
void exec_manualSelectFile(void)
{
		char *fliename;
		FIL readFile;
		printf("start \r\n");
	
		fliename =mymalloc(SRAMIN,20);//动态分配空间
		memset(fliename,'\0',strlen(fliename));
		//带UI的文件选择
		selectFileFromSD(fliename);
		
		//获取的文件名
		printf(">>>>filemane:%s \r\n",fliename);
		
		//获取文件变量
		getFile(&readFile,fliename);
		myfree(SRAMIN,fliename);

		//带UI的执行文件
		nc_excuteFile(&readFile);
}
