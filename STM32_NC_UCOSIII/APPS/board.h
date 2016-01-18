#ifndef __BOARD_H
#define __BOARD_H

	#include "sys.h"
	#include "delay.h"
	#include "usart.h"
	#include "led.h"
	#include "lcd.h"
	#include "beep.h"
	#include "APPS.h"
	
	#include "key.h"  
	#include "sram.h"   
	#include "malloc.h"  
	#include "sdio_sdcard.h"    
	#include "malloc.h" 
	#include "w25qxx.h"    
	#include "ff.h"  
	#include "exfuns.h"  
	#include "fattester.h" 
	
	#include "NCSupport.h"
	#include "APPInterface.h"

		#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)
		#define SYSRESETREQ_MASK (2<<1)

	void borad_HarwareInit(void);
	
	//SD检测等
	void SDCard_Init(void);
	//串口接收数据保存在SD卡
	void m_creatFile(char * pathname ,int timeout,u16 cycletime);
	//串口状态条
	void com_statusBar(char * comdata);
	
	void SystemReset(void);

#endif
