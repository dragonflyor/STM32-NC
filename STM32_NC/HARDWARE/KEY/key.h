#ifndef __KEY_H
#define __KEY_H

	#include "stm32f10x.h"
	#include "sys.h"
	
	#define KEY0 PAin(13)
	#define KEY1 PAin(15)
	#define WK_UP PAin(0)
	
	#define KEY0_PRES 1
	#define KEY1_PRES 2
	#define WKUP_PRES 3
	#define KEY_NULL 0
	
	void KEY_Init(void);
	u8 KEY_Scan(u8 mode);
	
	
#endif



