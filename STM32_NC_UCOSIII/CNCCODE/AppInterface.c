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


//输入文件名称直接执行
void exec_byFilename(char *filename)
{
			FIL readFile;
	
		//获取文件变量
		getFile(&readFile,filename);

		//带UI的执行文件
		nc_excuteFile(&readFile);

}


//处理串口传来的信息
void com_Process(void)
{
				//-----------------------------------------------------------------------
		//成功接收到数据
		if(USART_RX_STA&0x8000)
		{
				//接收到的数据长度
				int len=USART_RX_STA&0x3fff;
				char * conn_recv;

				//开辟内存
				conn_recv = (char*)mymalloc(SRAMIN,len+1);
				mymemset(conn_recv,0,len+1);	//设置内存
				
				//取串口的数据 
				mymemcpy(conn_recv, USART_RX_BUF,len);
				
				//状态复位
				USART_RX_STA=0;
				
				//显示串口状态到屏幕
				com_statusBar(conn_recv);
				
			//协议码">>01+filename":接收文件
				if(conn_recv[0]=='>' && conn_recv[1]=='>' && conn_recv[2]=='0' && conn_recv[3]=='1')
				{
					char * filename = (char *)mymalloc(SRAMIN,20);
					mymemset(filename,0,strlen(filename));
					mymemcpy(filename,&conn_recv[4],len-4);//复制内存   
					printf("parsed filename : %s \r\n",filename);
					
						printf("收文件\r\n");
						//接收文件
					//第一个参数：要保存的文件名，默认保存地址为0:/NCCODE/
						//第二个参数：连续轮询多次（100次），都没数据进来 ，则接收文件超时，超时时间T=100*200ms
						//第三个参数：轮询周期
						m_createFileDef(filename,100,200);
						printf("结束收文件\r\n");
				}
				//协议码">>02+filename":接收文件并执行
				else if(conn_recv[0]=='>' && conn_recv[1]=='>' && conn_recv[2]=='0' && conn_recv[3]=='2')
				{
					int key = 0;
					char * filename = (char *)mymalloc(SRAMIN,20);
					mymemset(filename,0,strlen(filename));
					mymemcpy(filename,&conn_recv[4],len-4);//复制内存   
					printf("parsed filename : %s \r\n",filename);
					
					//1.接收文件
					m_createFileDef(filename,100,200);
						//OS_CRITICAL_ENTER();
					//2.执行文件
						exec_byFilename(filename);
						myfree(SRAMIN,filename);
						//按了向上的键则返回菜单
						key = KEY_Scan(0);
						while(key != WKUP_PRES)
						{
							key = KEY_Scan(0);
						}
						//系统复位
						SystemReset();
						//OS_CRITICAL_EXIT();
				}

				//释放内存
				myfree(SRAMIN,conn_recv); 	
		}
		//-----------------------------------------------------------------------
}

