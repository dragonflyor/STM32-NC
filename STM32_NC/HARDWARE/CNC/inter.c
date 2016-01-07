#include "inter.h"
#include "math.h"
#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "usart.h"

#define  NUM_AXIS 4
extern float current_position[NUM_AXIS];
extern float destination[NUM_AXIS];
extern long int  steps;
int f=0;
char mode;
char humode;//圆弧优弧和劣弧标志
float O_x,O_y;
int inR;

	#define RUN_POS 1   //正转
	#define RUN_NEG 0		//反转

	#define XAXIS  1  //正反转时指定轴
	#define YAXIS  2

//用B2..7驱动
#define X_DIR_PIN PAout(8)//X Y Z 三个轴的方向信号引脚定义
#define Y_DIR_PIN PAout(11)
#define Z_DIR_PIN PAout(12)

#define X_PUL_PIN PAout(4)//X Y Z 三个轴的脉冲信号引脚定义
#define Y_PUL_PIN PCout(4)
#define Z_PUL_PIN PCout(5)

//限位开关位置
#define X_ORG PAout(1)//X Y Z 三个轴的起点信号引脚定义
#define Y_ORG PAout(4)
#define Z_ORG PAout(5)

extern int x,y;

extern u16 Temp_X,Temp_Y; //窗口坐标平移

//原点判别标志
char IS_XOrg=0;
char IS_YOrg=0;
char IS_ZOrg=0;

//关于电机驱动的接口初始化
void driver_GPIOInit(void ){
	GPIO_InitTypeDef GPIO_InitTypeStruc;
	NVIC_InitTypeDef NVIC_InitTypeStruc;
	EXTI_InitTypeDef EXTI_InitTypeStruc;
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
//	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOB,&GPIO_InitTypeStruc);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitTypeStruc);
	
	//回零接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitTypeStruc);
	
	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruc);
	
	EXTI_InitTypeStruc.EXTI_Line=EXTI_Line1;
	EXTI_InitTypeStruc.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitTypeStruc.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitTypeStruc.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitTypeStruc);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	
	EXTI_InitTypeStruc.EXTI_Line=EXTI_Line4;
	EXTI_InitTypeStruc.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitTypeStruc.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitTypeStruc.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitTypeStruc);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
	
	EXTI_InitTypeStruc.EXTI_Line=EXTI_Line5;
	EXTI_InitTypeStruc.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitTypeStruc.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitTypeStruc.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitTypeStruc);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
	
NVIC_InitTypeStruc.NVIC_IRQChannel=EXTI0_IRQn;
NVIC_InitTypeStruc.NVIC_IRQChannelPreemptionPriority=0x00;
NVIC_InitTypeStruc.NVIC_IRQChannelSubPriority=0x00;
NVIC_InitTypeStruc.NVIC_IRQChannelCmd=ENABLE;
NVIC_Init(&NVIC_InitTypeStruc);

NVIC_InitTypeStruc.NVIC_IRQChannel=EXTI4_IRQn;
NVIC_InitTypeStruc.NVIC_IRQChannelPreemptionPriority=0x00;
NVIC_InitTypeStruc.NVIC_IRQChannelSubPriority=0x00;
NVIC_InitTypeStruc.NVIC_IRQChannelCmd=ENABLE;
NVIC_Init(&NVIC_InitTypeStruc);

NVIC_InitTypeStruc.NVIC_IRQChannel=EXTI9_5_IRQn;
NVIC_InitTypeStruc.NVIC_IRQChannelPreemptionPriority=0x00;
NVIC_InitTypeStruc.NVIC_IRQChannelSubPriority=0x00;
NVIC_InitTypeStruc.NVIC_IRQChannelCmd=ENABLE;
NVIC_Init(&NVIC_InitTypeStruc);
}
//圆弧插补的圆心
void GetO(int x0,int y0,int xe,int ye,int inR)
{
	int StartX = x0;//??X??
	int StartY = y0;//??Z??
	int EndX = xe;//??X??
	int EndY = ye;//??Z??
	int Osteps1,Osteps2;
	double Radius = inR;//??
	double MidpointX = (StartX + EndX)/2.0;//??X??
	double MidpointY = (StartY + EndY)/2.0;//??Y??
	double TempI = (StartX - MidpointX)*(StartX - EndX);
	double TempJ = (StartX - EndX)*(StartX - EndX);
	double TempK = Radius*Radius - StartY*StartY;
	double TempL = StartY - EndY;
	double TempM = TempI - MidpointY*TempL;
	double TempN = (TempL * TempL + TempJ)*(TempK*TempJ - TempM*TempM);
	double TempO = TempL*TempL + TempJ; 
	double TempP = TempM*TempL - StartY*TempJ;
	double X1;
	double X2;//??X??
	double Y1;
	double Y2;//??Z??
	Y1 = (sqrt((long double)((TempN + TempP*TempP)/(TempO*TempO)))) - (TempP/TempO);
	X1 = MidpointX + ((StartY - EndY) * (Y1 - MidpointY))/(EndX - StartX);
	//printf("The Circle X1 coordinate value is %d.\r\n", (int)X1);
	//printf("The Circle Y1 coordinate value is %f.\r\n", Y1); 
	Y2 = (-sqrt((long double)((TempN + TempP*TempP)/(TempO*TempO)))) - (TempP/TempO);
	X2 = MidpointX + ((StartY - EndY) * (Y2 - MidpointY))/(EndX - StartX);
	//printf("The Circle X2 coordinate value is %f.\r\n", X2);
	//printf("The Circle Z2 coordinate value is %f.\r\n", Y2);
	
	
	{
		//O_x=(int)(X1+0.5);//????
		//O_y=(int)(Y1+0.5);
		O_x=X1;
		O_y=Y1;
		GetSteps(mode);
		Osteps1=steps;
 	}
 	{
 		//O_x=(int)(X2+0.5);//????
 		//O_y=(int)(Y2+0.5);
		O_x=X2;
		O_y=Y2;
 		GetSteps(mode);
 		Osteps2=steps;
 	}
 	if(humode==1)
 	{
 		if(Osteps1>Osteps2)
 		{
// 			O_x=(int)(X1+0.5);//????
// 			O_y=(int)(Y1+0.5);
				O_x=X1;
				O_y=Y1;
 		}
 		else
 		{
// 			O_x=(int)(X2+0.5);//????
// 			O_y=(int)(Y2+0.5);
		O_x=X2;
		O_y=Y2;
 		}
 	}
 	else if(humode==0)
 	{
 		if(Osteps1<Osteps2)
 		{
// 			O_x=(int)(X1+0.5);//????
// 			O_y=(int)(Y1+0.5);
				O_x=X1;
		O_y=Y1;
 		}
 		else
 		{
// 			O_x=(int)(X2+0.5);//????
// 			O_y=(int)(Y2+0.5);
				O_x=X2;
		O_y=Y2;
 		}
 	}
}

u32 GetSteps(char mode)
{

	
		u8 a,b;
	double steps1=0,steps2=0;
	
	if(mode=='c')
	{
		a=quad_test((current_position[0]-O_x),(current_position[1]-O_y));
		b=quad_test((destination[0]-O_x),(destination[1]-O_y));
	
		switch(a)
		{
			case 1:
				steps1=inR-(current_position[0]-O_x)+(current_position[1]-O_y)-0;
				break;
			case 2:
				steps1=2*inR+(0-(current_position[0]-O_x))+(inR-(current_position[1]-O_y));
				break;
			case 3:
				steps1=4*inR+((current_position[0]-O_x)+inR)+(0-(current_position[1]-O_y));
				break;
			case 4:
				steps1=6*inR+((current_position[0]-O_x)-0)+((current_position[1]-O_y)+inR);
				break;
		}
		switch(b)
		{
			case 1:
				steps2=(inR-(destination[0]-O_x))+((destination[1]-O_y)-0);
				break;
			case 2:
				steps2=2*inR+(0-(destination[0]-O_x))+(inR-(destination[1]-O_y));
				break;
			case 3:
				steps2=4*inR+((destination[0]-O_x)+inR)+(0-(destination[1]-O_y));
				break;
			case 4:
				steps2=6*inR+((destination[0]-O_x)-0)+((destination[1]-O_y)+inR);
				break;
		}
		//测试
		printf("\r\nsteps1:%f   steps2:%f\r\n",steps1,steps2);
		
		//if((mode=='c'))
		{
		if(steps2>=steps1)
			steps=steps2-steps1+0.5;
		else
			steps=(8*inR)-steps1+steps2+0.5;
		}
	

	}
		if(mode=='n')
	{
		a=quad_test((current_position[0]-O_x),(current_position[1]-O_y));
		b=quad_test((destination[0]-O_x),(destination[1]-O_y));
	
		switch(a)
		{
			case 1:
				steps1=6*inR+(current_position[0]-O_x)+(inR-(current_position[1]-O_y));
				break;
			case 2:
				steps1=4*inR+(inR+(current_position[0]-O_x))+(current_position[1]-O_y);
				break;
			case 3:
				steps1=2*inR-(current_position[0]-O_x)+(inR+(current_position[1]-O_y));
				break;
			case 4:
				steps1=-(current_position[1]-O_y)+(inR-(current_position[0]-O_x));
				break;
		}
		switch(b)
		{
			case 1:
				steps2=6*inR+(destination[0]-O_x)+(inR-(destination[1]-O_y));
				break;
			case 2:
				steps2=4*inR+(inR+(destination[0]-O_x))+(destination[1]-O_y);
				break;
			case 3:
				steps2=2*inR-(destination[0]-O_x)+(inR+(destination[1]-O_y));
				break;
			case 4:
				steps2=-(destination[1]-O_y)+(inR-(destination[0]-O_x));
				break;
		}
		
		if(steps2>=steps1)
			steps=steps2-steps1+0.5;
		else
			steps=(8*inR)-steps1+steps2+0.5;
	
	}
	
	
	if(mode=='l')

	{
		steps=(int )fabs(current_position[0]-destination[0])+(int)fabs(current_position[1]-destination[1]);

	}
	
	return steps;
}

void line_inter(){
	
//		P15=!P15;
int aa=0;//与LCD轨迹显示密度有关
	while(steps-->0)
   {
//		 int test=1;
//    int x_pre=x,y_pre=y;
// 		 int k[2]={1,1};//LCD正负号处理系数
		 
		
		//delay_ms(50);
   switch(quad_test((x-current_position[0]),(y-current_position[1])))
		//switch(1)//-------------------------------------------------test
		{
			
			case 1: 
				if(f>=0)
				{ 
					x++;
					run_pos(XAXIS);
					
					f=f-(destination[1]-current_position[1]);
				}
				else
				{
					y++; 
					run_pos(YAXIS);
					f=f+(destination[0]-current_position[0]);


				}
				break;	

			case 2:
				//if(f>=0)
				if(f<0)
				{
					x--;
					run_neg(XAXIS); 
					f=f+(destination[1]-current_position[1]);
				}
				else
				{
					y++;
					run_pos(YAXIS);
					f=f+(destination[0]-current_position[0]);
				
				}								
				break;

			case 3:
				if(f>0)
				{
				
					y--;
					run_neg(YAXIS); 
					
					f=f+(destination[0]-current_position[0]);
				}
				else
				{
					
					x--;
					run_neg(XAXIS); 
					
					f=f-(destination[1]-current_position[1]);
				}
				break;

			case 4:
				if(f>=0)
				{
					
					y--;
					run_neg(YAXIS); 
					f=f-(destination[0]-current_position[0]);

				}
				else
				{
					
					x++;
					run_pos(XAXIS);
					f=f-(destination[1]-current_position[1]);
				}
				break;
		}
// 		
// 			 LCD_DrawLine(lcddev.width/2+x_pre, lcddev.width-(lcddev.width/2+y_pre), 
// 								lcddev.width/2+x, lcddev.width-(lcddev.width/2+y));
// 		
// 	if(x<0)
// 	{ 
// 		k[0]=-1;
// 				LCD_ShowChar(Temp_X+8*3,Temp_Y,'-',16,0);//显示负号
// 	}
// 	else
// 	{
// 		k[0]=1;
// 		LCD_ShowChar(Temp_X+8*3,Temp_Y,' ',16,0);//不显示负号
// 	}
// 		if(y<0)
// 	{ 
// 		k[1]=-1;
// 				LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,'-',16,0);//显示负号
// 	}
// 		else
// 	{
// 		k[1]=1;
// 		LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,' ',16,0);//不显示负号
// 	}
// 		
// 	//刷新坐标x y	
// 	LCD_ShowNum(Temp_X+8*4, Temp_Y,x*k[0],5,16);
// 	LCD_ShowNum(Temp_X+8*15, Temp_Y,y*k[1],5,16);
// 	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,steps,6,16);

			if(aa++>2)//隔3步显示一个点
			{
				aa=0;
				updataCurrentXY();//刷新LCD上的显示
			}
		

   }
	
}

void circ_inter()
{
	int aa=0;//与LCD轨迹显示密度有关
		GetSteps(mode);
	//	printf("\r\nStarting  Circ   Steps:%ld!!\r\n",steps);
	//printf("\r\n(x0,y0),(xe,ye):(%d,%d),(%d,%d)\r\n",
	//(int)current_position[0],(int)current_position[1],(int)destination[0],(int)destination[1]);
	
		//steps=101;
		while(steps-->0)
		{
			
		//printf("\r\nStarting  Circ!!\r\n");
		switch(quad_test((x-O_x),(y-O_y)))
			{
				case 1:
					if(f>=0)
					{
						//run_neg(1);//xmotor a step back
						f=f-2*(x-O_x)+1;
						x--;
						
						run_neg(XAXIS); 
					}
					else
					{
						//run_pos(2);//ymotor a step forward
						f=f+2*(y-O_y)+1;
						y++;
						run_pos(YAXIS);
						
					}
					break;	

				case 2:
					if(f>=0)
					{
						//run_neg(2);//ymotor a step back
						f=f-2*(y-O_y)+1;
						y--;
						run_neg(YAXIS); 
						
					}
					else
					{
						//run_neg(1);//xmotor a step back
						f=f-2*(x-O_x)+1;
						x--;
						run_neg(XAXIS); 
					}								
					break;

				case 3:
					if(f>=0)
					{
						//run_pos(1);//xmotor a step forward
						f=f+2*(x-O_x)+1;
						x++;
						run_pos(XAXIS);
					}
					else
					{
						//run_neg(2);//ymotor a step back
						f=f-2*(y-O_y)+1;
						y--;
						run_neg(YAXIS);
					}
					break;
					
				case 4:
					if(f>=0)
					{
						//run_pos(2);//ymotor a step forward
						f=f+2*(y-O_y)+1;
						y++;
						run_pos(YAXIS);
					}
					else
					{
						//run_pos(1);//xmotor a step forward
						f=f+2*(x-O_x)+1;
						x++;
						run_pos(XAXIS);
					}								
					break;
			}
			
			if(aa++>2)//隔3步显示一个点
			{
				aa=0;
				updataCurrentXY();//刷新LCD上的显示
			}
		
		
//到达终点结束			
		if(((int)(destination[0]-x)==0&&(int)(destination[1]-y)==0)){return;}
		//	delay_ms(2000);
		}
		
		
		if(((int)(destination[0]-x)!=0)||((int)(destination[1]-y)!=0)){
			//步数计算误差补偿
//			int steps_compensate=0;
//			steps_compensate=(int )fabs(x-destination[0])+(int )fabs(y-destination[0]);
//			steps=steps_compensate;
//			current_position[0]=x;
//			current_position[1]=y;
//			circ_inter();
			steps=0;
			//最后一步误差处理
			switch((int)(destination[0]-x))
			{
					case 0: break;
					case 1: run_pos(XAXIS); x++; break;
					case -1: run_neg(XAXIS); x--;break;
					default :printf("\r\nerror：XAXIS 补偿 错误！\r\n");break;
			}
					switch((int)(destination[1]-y))
			{
					case 0: break;
					case 1: run_pos(YAXIS); y++; break;
					case -1: run_neg(YAXIS); y--;break;
					default :printf("\r\nerror：YAXIS 补偿 错误！\r\n");break;
			}
			
			updataCurrentXY();//刷新LCD上的显示
	}
		else return;

}

void circr_inter()
{
	int aa=0;
		while(steps-->0)
		{
			
		switch(quad_test((x-O_x),(y-O_y)))
			{
				case 1:
					if(f<0)
					{
						//run_neg(1);//xmotor a step back
						f=f+2*(x-O_x)+1;
						x++;
						
					}
					else
					{
						//run_pos(2);//ymotor a step forward
						f=f-2*(y-O_y)+1;
						y--;
						
					}
					break;	

				case 2:
					if(f<0)
					{
						//run_neg(2);//ymotor a step back
						f=f+2*(y-O_y)+1;
						y++;
						
					}
					else
					{
						//run_neg(1);//xmotor a step back
						f=f+2*(x-O_x)+1;
						x++;
						
					}								
					break;

				case 3:
					if(f<0)
					{
						//run_pos(1);//xmotor a step forward
						f=f-2*(x-O_x)+1;
						x--;
						
					}
					else
					{
						//run_neg(2);//ymotor a step back
						f=f+2*(y-O_y)+1;
						y++;
						
					}
					break;
					
				case 4:
					if(f<0)
					{
						//run_pos(2);//ymotor a step forward
						f=f-2*(y-O_y)+1;
						y--;
						
					}
					else
					{
						//run_pos(1);//xmotor a step forward
						f=f-2*(x-O_x)+1;
						x--;
						
					}								
					break;

			}
					if(aa++>2)//隔3步显示一个点
			{
				aa=0;
				updataCurrentXY();//刷新LCD上的显示
			}
			//到达终点结束			
		if(((int)(destination[0]-x)==0&&(int)(destination[1]-y)==0)){return;}
		}
		
			if(((int)(destination[0]-x)!=0||(int)(destination[1]-y)!=0)){
			//步数计算误差补偿
			int steps_compensate=0;	
			steps_compensate=(int )fabs(x-destination[0])+(int )fabs(y-destination[0]);
			steps=steps_compensate;
			current_position[0]=x;
			current_position[1]=y;
				mode='n';
			//circr_inter();
			circr_inter();
				
			steps=0;
			//最后一步误差处理
			switch((int)(destination[0]-x))
			{
					case 0: break;
					case 1: run_pos(XAXIS); x++; break;
					case -1: run_neg(XAXIS); x--;break;
					default :printf("\r\nerror：XAXIS 补偿 错误！\r\n");break;
			}
					switch((int)(destination[1]-y))
			{
					case 0: break;
					case 1: run_pos(YAXIS); y++; break;
					case -1: run_neg(YAXIS); y--;break;
					default :printf("\r\nerror：YAXIS 补偿 错误！\r\n");break;
			}
			
			updataCurrentXY();//刷新LCD上的显示
	}
}

int quad_test(int x,int y)
{
 	switch(mode)
 		{
		case 'c':if((x>0&&y>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
			return (1);

			if(x<=0&&y>0)                      //quad 2	and y+axis
				return (2);
		
				 if(x<0&&y<=0)                      //quad 3 and x-axis	
					return (3);
							
					 if(x>=0&&y<0)                      //quad 4	and y-axis
						return (4);
				break;
		case 'n':if((x>=0&&y>0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
			return (1);

			if(x<0&&y>=0)                      //quad 2	and y+axis
				return (2);
		
				 if(x<=0&&y<0)                      //quad 3 and x-axis	
					return (3);
							
					 if(x>0&&y<=0)                      //quad 4	and y-axis
						return (4);
				break;
					 
 		case 'l'://???20150331??
			if((x>0&&y>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
				return (1);

			if(x<=0&&y>0)                      //quad 2	and y+axis
				return (2);
		
			if(x<0&&y<=0)                      //quad 3 and x-axis	
				return (3);
							
			if(x>=0&&y<0)                      //quad 4	and y-axis
				return (4);


			if(x==0||y==0)
						 //if((x==0&&y==0))
						{
						
							if(((destination[0]-current_position[0])>0&&(destination[1]-current_position[1])>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
								return (1);

							else if((destination[0]-current_position[0])<=0&&(destination[1]-current_position[1])>0)                      //quad 2	and y+axis
								return (2);
												
							else if((destination[0]-current_position[0])<0&&(destination[1]-current_position[1])<=0)                      //quad 3 and x-axis	
								return (3);
													
							else if((destination[0]-current_position[0])>=0&&(destination[1]-current_position[1])<0)                      //quad 4	and y-axis
								return (4);

							else if(destination[0]==0&&destination[1]==0)    //???????? ????
							{
											if(((destination[0]-current_position[0])>0&&(destination[1]-current_position[1])>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
												return (1);

												else if((destination[0]-current_position[0])<=0&&(destination[1]-current_position[1])>0)                      //quad 2	and y+axis
													return (2);
											
													else if((destination[0]-current_position[0])<0&&(destination[1]-current_position[1])<=0)                      //quad 3 and x-axis	
														return (3);
												
														else if((destination[0]-current_position[0])>=0&&(destination[1]-current_position[1])<0)                      //quad 4	and y-axis
															return (4);
							}

						}
						break;//--------------------------------------------------------------------------
		}

		return 0;
}

 void run_pos(u8 ii)
 {
//	 P14=!P14;

	switch(ii)
	{
		case 1:
			X_DIR_PIN=RUN_POS;
			delay_us(10);
			X_PUL_PIN=0;
		delay_us(10);
			X_PUL_PIN=1;
			break;
		case 2:
			Y_DIR_PIN=RUN_POS;
			delay_us(10);
			Y_PUL_PIN=0;
			delay_us(10);
			Y_PUL_PIN=1;
			break;
	}
	 

 }

	void run_neg(u8 jj)
{

	switch(jj)
	{
		case 1:
			X_DIR_PIN=RUN_NEG;
			delay_us(10);
			X_PUL_PIN=0;
			delay_us(10);
			X_PUL_PIN=1;
			break;
		case 2:
			Y_DIR_PIN=RUN_NEG;;
			delay_us(10);
			Y_PUL_PIN=0;
			delay_us(10);
			Y_PUL_PIN=1;
			break;
	}
	 
}

void updataCurrentXY(){
//	  int x_pre=x,y_pre=y;
		 int k[2]={1,1};//LCD正负号处理系数
//绘制轨迹点
//				 LCD_DrawLine(lcddev.width/2+x_pre, lcddev.width-(lcddev.width/2+y_pre), 
//								lcddev.width/2+x, lcddev.width-(lcddev.width/2+y));
 //LCD_DrawLine(lcddev.width/2+x, lcddev.width-(lcddev.width/2+y), 
//							lcddev.width/2+x, lcddev.width-(lcddev.width/2+y));

LCD_DrawPoint(lcddev.width/2+x,lcddev.width-(lcddev.width/2+y));	
		
	if(x<0)
	{ 
		k[0]=-1;
				LCD_ShowChar(Temp_X+8*3,Temp_Y,'-',16,0);//显示负号
	}
	else
	{
		k[0]=1;
		LCD_ShowChar(Temp_X+8*3,Temp_Y,' ',16,0);//不显示负号
	}
		if(y<0)
	{ 
		k[1]=-1;
				LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,'-',16,0);//显示负号
	}
		else
	{
		k[1]=1;
		LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,' ',16,0);//不显示负号
	}
		
	//刷新坐标x y	
	LCD_ShowNum(Temp_X+8*4, Temp_Y,x*k[0],5,16);
	LCD_ShowNum(Temp_X+8*15, Temp_Y,y*k[1],5,16);
	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,steps,6,16);


}

void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0)!=RESET){
		IS_XOrg=1;
		
		EXTI_ClearITPendingBit(EXTI_Line0);
	}

}
void EXTI4_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET){
		IS_YOrg=1;
		
		EXTI_ClearITPendingBit(EXTI_Line4);
	}

}
void EXTI9_5_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET){
		IS_ZOrg=1;
		
		EXTI_ClearITPendingBit(EXTI_Line5);
	}

}

void axisToZero(){
	while((!IS_XOrg)||(!IS_YOrg)||(!IS_YOrg))//有轴不在原点
	{
		if(!IS_XOrg)
		{
			run_neg(XAXIS);
		}
		
			if(!IS_YOrg)
		{
			run_neg(YAXIS);
		}
		
				if(!IS_ZOrg)
		{
			//Z反转一个脉冲
			//run_neg(ZAXIS);
		}
		
		delay_ms(50);//脉冲周期
	}

}
