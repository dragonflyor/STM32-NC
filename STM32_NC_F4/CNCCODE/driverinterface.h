#ifndef __DRIVERINTERFACE_H
#define __DRIVERINTERFACE_H

#include "stm32f10x.h"

void driver_GPIOInit1(void );
void interpolationTaskDelet(void);
void interpolationChangeSpeed(unsigned int period);
void tim_Init(uint16_t  TIM_Peri,uint16_t  TIM_Pres );

	void run_neg1(u8 jj);
	void run_pos1(u8 ii);

#endif
