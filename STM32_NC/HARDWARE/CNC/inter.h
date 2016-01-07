#ifndef __INTER_H
#define __INTER_H
	
	#include "stm32f10x.h"

u32 GetSteps(char mode);
int quad_test(int x,int y);
void line_inter(void);
void circ_inter(void);
void circr_inter(void);
void driver_GPIOInit(void );
void run_pos(u8 ii);
void run_neg(u8 jj);
void GetO(int x0,int y0,int xe,int ye,int inR);
void updataCurrentXY(void);

#endif
