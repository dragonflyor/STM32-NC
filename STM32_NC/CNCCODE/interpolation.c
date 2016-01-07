#include "interpolation.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "stdbool.h"
#include "usart.h"
#include "malloc.h"
#include "driverinterface.h"


char *strchr_pointer; //保存关键字的地址指针

//调试选择 1插补使用串口输出调试 0不使用
#define USE_INTER_DEBUG (0)

#if  USE_INTER_DEBUG

#endif

#define NUM_AXIS 4
char axis_codes1[NUM_AXIS]= {'X', 'Y', 'Z', 'E'};//轴在G代码中的关键字

#define RUN_POS 1   //正转
#define RUN_NEG 0		//反转

#define XAXIS  1  //正反转时指定轴
#define YAXIS  2


long int GetSteps1(NC_Para* pNC_Para);

extern char *strchr_pointer;
extern NC_Para* pNC_Para;
extern GCode_Msg* pGCode_Msg;
extern u8 ZOOM;

//函数：code_value
//功能：获取字符串中指定字符指针strchr_pointer后面的数字参数，用于获取G代码参数值
//输入：关键字的指针strchr_pointer
//返回：G代码中关键字后面的数值
 float code_value1(char* strchr_pointer) 
{ 

	#if  USE_INTER_DEBUG
	printf("0------%s\r\n",(const char*)strchr_pointer);
	printf("1------%f\r\n",(float)strtod((const char*)(strchr_pointer+1),NULL));
	printf("2------%f\r\n",strtod("1.34569",NULL));
	#endif
	
	return ((float)strtod((strchr_pointer+1), NULL));
}

//函数：code_seen
//功能：查找字符串strcmdbuffer中的关键字code
//输入：关键字code
//返回：查找到关键字1 否则0
 bool code_seen1(char code,char *strcmdbuffer)
	{



		strchr_pointer = strchr(strcmdbuffer, (int)code);
		
	#if  USE_INTER_DEBUG
		printf("code_seen找到%c %d\r\n",code,(strchr_pointer != NULL));
		printf("code_seen找到%s\r\n",(char*)(strchr_pointer) );
	#endif
	
		return ((strchr_pointer != NULL));	//Return True if a character was found
	}
	


//函数：get_coordinatesValue
//功能：查找字符串strcmdbuffer中的坐标值 XYZ R
//输入：字符串strcmdbuffer 参数结构体指针pNC_Para
//输出：G代码信息结构体指针pGCode_Msg
 void get_coordinatesValue1(char* strcmdbuffer,GCode_Msg* pGCode_Msg)
 {
	 int i;

	#if  USE_INTER_DEBUG
		printf("get_coordinatesValues输入%s\r\n",(char*)(strcmdbuffer) );
	#endif
	
	
	 
	 //解释保存坐标值 XYZ
 	 for(i=0; i < NUM_AXIS; i++) {
			if(code_seen1(axis_codes1[i],strcmdbuffer)) {
	#if  USE_INTER_DEBUG
		printf("111--:%c %s",axis_codes1[i],(char*)strchr_pointer);
	#endif
				pGCode_Msg->coor_value[i]= (float)code_value1(strchr_pointer)*ZOOM;//+ (axis_relative_modes[i] || relative_mode)*current_position[i];
			}
			else //pGCode_Msg->coor_value[i] = pNC_Para->coor_currentValue[i];      //Are these else lines really needed?
				pGCode_Msg->coor_value[i] = pGCode_Msg->coor_value[i]; 
	 }
	//解释保存半径 R
	 if(code_seen1('R',strcmdbuffer)){
			pGCode_Msg->O_R=code_value1(strchr_pointer)*ZOOM;
		}
		
 }
	
//函数：decode
//功能：对一行字符指令进行译码，译码得到的信息保存在pGCode_Msg结构体中
//输入：字符串strcmdbuffer 
//输出：G代码信息结构体指针pGCode_Msg
void decode(char* strcmdbuffer )
{
	
	#if  USE_INTER_DEBUG
		printf("DECODE---%s\n",strcmdbuffer);
	#endif
	if(code_seen1('G',strcmdbuffer))//G??
   {
		 	
	#if  USE_INTER_DEBUG
	   printf("22222G:%d\r\n",(int)code_value1(strchr_pointer));
	#endif

      switch((int)code_value1(strchr_pointer))
      {
				
				case 0: // G0
				case 1: // G1
				{
						pGCode_Msg->G_instruct=0x01;
						get_coordinatesValue1(strcmdbuffer,pGCode_Msg);//获取 XYZR 坐标值
						break;
				} 
				 case 2://G2 逆圆弧插补
				{
					 pGCode_Msg->G_instruct=0x02;
					get_coordinatesValue1(strcmdbuffer,pGCode_Msg);//获取 XYZR 坐标值
					 break;
				}

				 case 3://G3 顺圆弧插补
				{
						 pGCode_Msg->G_instruct=0x03;
						get_coordinatesValue1(strcmdbuffer,pGCode_Msg);//获取 XYZR 坐标值
					 break;
				}		 
      }
   }
	 else if(code_seen1('M',strcmdbuffer))//保存M指令
	 {
			pGCode_Msg->M_instruct=(int)code_value1(strchr_pointer);;
	 }
	 else if(code_seen1('F',strcmdbuffer))//保存速度F
	 {
			 pGCode_Msg->F = code_value1(strchr_pointer);
	 }
}

// //函数：quad_test
// //功能：插补方式G_instruct下x,y所在象限
// //输入：坐标点x,y 插补方式G_instruct
// //返回：象限值
int quad_test2(long x,long y,char G_instruct)
{


	
 	switch(G_instruct)
 		{
			
		case 0x00:	 
 		case 0x01://
			if((x>0&&y>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
				return (1);

			if(x<=0&&y>0)                      //quad 2	and y+axis
				return (2);
		
			if(x<0&&y<=0)                      //quad 3 and x-axis	
				return (3);
							
			if(x>=0&&y<0)                      //quad 4	and y-axis
				return (4);
			
			return 0;

			
		case 0x02:
			if((x>0&&y>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
			return (1);

			if(x<=0&&y>0)                      //quad 2	and y+axis
				return (2);
		
				 if(x<0&&y<=0)                      //quad 3 and x-axis	
					return (3);
							
					 if(x>=0&&y<0)                      //quad 4	and y-axis
						return (4);
				break;
					 
		case 0x03:
			if((x>=0&&y>0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
			return (1);

			if(x<0&&y>=0)                      //quad 2	and y+axis
				return (2);
		
				 if(x<=0&&y<0)                      //quad 3 and x-axis	
					return (3);
							
					 if(x>0&&y<=0)                      //quad 4	and y-axis
						return (4);
				break;
					 
//		case 0x00:	 
// 		case 0x01://
//			if((x>0&&y>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
//				return (1);

//			if(x<=0&&y>0)                      //quad 2	and y+axis
//				return (2);
//		
//			if(x<0&&y<=0)                      //quad 3 and x-axis	
//				return (3);
//							
//			if(x>=0&&y<0)                      //quad 4	and y-axis
//				return (4);


//			if(x==0||y==0)
//						 //if((x==0&&y==0))
//			{
//						
//							if(((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])>0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
//								return (1);

//							else if((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])<=0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])>0)                      //quad 2	and y+axis
//								return (2);
//												
//							else if((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])<0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])<=0)                      //quad 3 and x-axis	
//								return (3);
//													
//							else if((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])>=0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])<0)                      //quad 4	and y-axis
//								return (4);

//							else if(pNC_Para->coor_destinationValue[0]==0&&pNC_Para->coor_destinationValue[1]==0)    //???????? ????
//							{
//											if(((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])>0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])>=0))//||(x==0&&y==0))      //quad 1 and (0,0)point	and x+axis
//												return (1);

//												else if((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])<=0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])>0)                      //quad 2	and y+axis
//													return (2);
//											
//													else if((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])<0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])<=0)                      //quad 3 and x-axis	
//														return (3);
//												
//														else if((pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])>=0&&(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])<0)                      //quad 4	and y-axis
//															return (4);
//							}

//			}
//						break;//--------------------------------------------------------------------------
		}

		return 0;
}


//函数：GetArcO
//功能：获取圆弧插补的圆心坐标，装载到插补参数结构体pNC_Para
//输入：插补参数结构体指针pNC_Para
//输出：插补参数结构体指针pNC_Para中的圆心值O_X O_Y
//调用步数函数GetSteps1 
void GetArcO1(NC_Para* pNC_Para)
{

	long int StartX=pNC_Para->coor_originValue[0];
	long int StartY=pNC_Para->coor_originValue[1];
	long int EndX = pNC_Para->coor_destinationValue[0];
	long int EndY=pNC_Para->coor_destinationValue[1];
	
	long int Osteps1,Osteps2;
	double Radius=pNC_Para->O_R;
	
	double MidpointX = (StartX + EndX)/2.0;
	double MidpointY= (StartY + EndY)/2.0;
	double TempI = (StartX - MidpointX)*(StartX - EndX);
	double TempJ = (StartX - EndX)*(StartX - EndX);
	double TempK = Radius*Radius - StartY*StartY;
	double TempL = StartY - EndY;
	double TempM = TempI - MidpointY*TempL;
	double TempN = (TempL * TempL + TempJ)*(TempK*TempJ - TempM*TempM);
	double TempO = TempL*TempL + TempJ; 
	double TempP = TempM*TempL - StartY*TempJ;
	double X1;
	double X2;
	double Y1;
	double Y2;
	
	//StartX=pNC_Para->coor_originValue[0];
	//StartY=pNC_Para->coor_originValue[1];
	//EndX = pNC_Para->coor_destinationValue[0];
	//EndY=pNC_Para->coor_destinationValue[1];
	//Radius=pNC_Para->O_R;
	//MidpointX = (StartX + EndX)/2.0;
	//MidpointX = (StartX + EndX)/2.0;
	
	Y1 = (sqrt((long double)((TempN + TempP*TempP)/(TempO*TempO)))) - (TempP/TempO);
	X1 = MidpointX + ((StartY - EndY) * (Y1 - MidpointY))/(EndX - StartX);

	Y2 = (-sqrt((long double)((TempN + TempP*TempP)/(TempO*TempO)))) - (TempP/TempO);
	X2 = MidpointX + ((StartY - EndY) * (Y2 - MidpointY))/(EndX - StartX);

	
		pNC_Para->O_X=X1;
		pNC_Para->O_Y=Y1;
		Osteps1=GetSteps1(pNC_Para);
#if  USE_INTER_DEBUG
	   printf("计算的圆心（O_x1,O_y1）:(%f,%f)\r\n",pNC_Para->O_X,pNC_Para->O_Y);
#endif
		pNC_Para->O_X=X2;
		pNC_Para->O_Y=Y2;
 		Osteps2=GetSteps1(pNC_Para);
#if  USE_INTER_DEBUG
	   printf("计算的圆心（O_x2,O_y2）:(%f,%f)\r\n",pNC_Para->O_X,pNC_Para->O_Y);
#endif
 	if(pNC_Para->humode==1)//优弧>180
 	{
 		if(Osteps1>Osteps2)
 		{
		pNC_Para->O_X=X1;
		pNC_Para->O_Y=Y1;
 		}
 		else
 		{
		pNC_Para->O_X=X2;
		pNC_Para->O_Y=Y2;
 		}
 	}
 	else if(pNC_Para->humode==0)//劣弧<180
 	{
 		if(Osteps1<Osteps2)
 		{
			pNC_Para->O_X=X1;
			pNC_Para->O_Y=Y1;
 		}
 		else
 		{
		pNC_Para->O_X=X2;
		pNC_Para->O_Y=Y2;
 		}
 	}
	//取整
	//pNC_Para->O_X=(pNC_Para->O_X>=0)?(long int )(pNC_Para->O_X+0.5):(long int )(pNC_Para->O_X-0.5);
	//pNC_Para->O_Y=(pNC_Para->O_Y>=0)?(long int )(pNC_Para->O_Y+0.5):(long int )(pNC_Para->O_Y-0.5);
	pNC_Para->O_X=(pNC_Para->O_X>=0)?(pNC_Para->O_X):(pNC_Para->O_X);
	pNC_Para->O_Y=(pNC_Para->O_Y>=0)?(pNC_Para->O_Y):(pNC_Para->O_Y);
#if  USE_INTER_DEBUG
	   printf("计算的圆心（O_x,O_y）:(%f,%f)\r\n",pNC_Para->O_X,pNC_Para->O_Y);
#endif
}

//函数：GetSteps
//功能：求插补起点到终点的插补步数
//输入：插补参数结构体指针pNC_Para
//返回：步数
//条件：在圆心函数之后使用，需要条用quad_test判断坐标所在象限
long int GetSteps1(NC_Para* pNC_Para)
{
	unsigned char a,b;
	long int steps1=0,steps2=0,steps;
	long int inR;
	long int O_x,O_y;
	

	
	inR=pNC_Para->O_R;
	O_x=pNC_Para->O_X;
	O_y=pNC_Para->O_Y;
	
	if(pNC_Para->G_instruct==0x02)//逆圆弧插补
	{

		a=quad_test2(pNC_Para->coor_originValue[0]-pNC_Para->O_X,
									pNC_Para->coor_originValue[1]-pNC_Para->O_Y,
									pNC_Para->G_instruct);
		b=quad_test2(pNC_Para->coor_destinationValue[0]-pNC_Para->O_X,
								pNC_Para->coor_destinationValue[1]-pNC_Para->O_Y,
								pNC_Para->G_instruct);
	
		switch(a)
		{
			case 1:
				steps1=inR-(pNC_Para->coor_originValue[0]-O_x)+(pNC_Para->coor_originValue[1]-O_y)-0;
				break;
			case 2:
				steps1=2*inR+(0-(pNC_Para->coor_originValue[0]-O_x))+(inR-(pNC_Para->coor_originValue[1]-O_y));
				break;
			case 3:
				steps1=4*inR+((pNC_Para->coor_originValue[0]-O_x)+inR)+(0-(pNC_Para->coor_originValue[1]-O_y));
				break;
			case 4:
				steps1=6*inR+((pNC_Para->coor_originValue[0]-O_x)-0)+((pNC_Para->coor_originValue[1]-O_y)+inR);
				break;
		}
		switch(b)
		{
			case 1:
				steps2=(inR-(pNC_Para->coor_destinationValue[0]-O_x))+((pNC_Para->coor_destinationValue[1]-O_y)-0);
				break;
			case 2:
				steps2=2*inR+(0-(pNC_Para->coor_destinationValue[0]-O_x))+(inR-(pNC_Para->coor_destinationValue[1]-O_y));
				break;
			case 3:
				steps2=4*inR+((pNC_Para->coor_destinationValue[0]-O_x)+inR)+(0-(pNC_Para->coor_destinationValue[1]-O_y));
				break;
			case 4:
				steps2=6*inR+((pNC_Para->coor_destinationValue[0]-O_x)-0)+((pNC_Para->coor_destinationValue[1]-O_y)+inR);
				break;
		}
		
		if(steps2>=steps1)
			steps=steps2-steps1+0.5;
		else
			steps=(8*inR)-steps1+steps2+0.5;
	}
	
	if(pNC_Para->G_instruct==0x03)//順圆弧插补
	{
		a=quad_test2(pNC_Para->coor_originValue[0]-pNC_Para->O_X,
									pNC_Para->coor_originValue[1]-pNC_Para->O_Y,
									pNC_Para->G_instruct);
		b=quad_test2(pNC_Para->coor_destinationValue[0]-pNC_Para->O_X,
								pNC_Para->coor_destinationValue[1]-pNC_Para->O_Y,
								pNC_Para->G_instruct);
	
		switch(a)
		{
			case 1:
				steps1=6*inR+(pNC_Para->coor_originValue[0]-O_x)+(inR-(pNC_Para->coor_originValue[1]-O_y));
				break;
			case 2:
				steps1=4*inR+(inR+(pNC_Para->coor_originValue[0]-O_x))+(pNC_Para->coor_originValue[1]-O_y);
				break;
			case 3:
				steps1=2*inR-(pNC_Para->coor_originValue[0]-O_x)+(inR+(pNC_Para->coor_originValue[1]-O_y));
				break;
			case 4:
				steps1=-(pNC_Para->coor_originValue[1]-O_y)+(inR-(pNC_Para->coor_originValue[0]-O_x));
				break;
		}
		switch(b)
		{
			case 1:
				steps2=6*inR+(pNC_Para->coor_destinationValue[0]-O_x)+(inR-(pNC_Para->coor_destinationValue[1]-O_y));
				break;
			case 2:
				steps2=4*inR+(inR+(pNC_Para->coor_destinationValue[0]-O_x))+(pNC_Para->coor_destinationValue[1]-O_y);
				break;
			case 3:
				steps2=2*inR-(pNC_Para->coor_destinationValue[0]-O_x)+(inR+(pNC_Para->coor_destinationValue[1]-O_y));
				break;
			case 4:
				steps2=-(pNC_Para->coor_destinationValue[1]-O_y)+(inR-(pNC_Para->coor_destinationValue[0]-O_x));
				break;
		}
		
		if(steps2>=steps1)
			steps=steps2-steps1+0.5;
		else
			steps=(8*inR)-steps1+steps2+0.5;
	
	}
	
	
	if((pNC_Para->G_instruct==0x01||pNC_Para->G_instruct==0x00))

	{
		steps=(long)fabs(pNC_Para->coor_originValue[0]-pNC_Para->coor_destinationValue[0])+(long)fabs(pNC_Para->coor_originValue[1]-pNC_Para->coor_destinationValue[1]);

	}
	
	return steps;
}


//函数：prepare_interpolation
//功能：将插补信息装载到pNC_Para,为插补准备好参数
//输入：代码信息结构体地址pGCode_Msg
//输出：插补信息结构体地址pNC_Para
void prepare_interpolation(GCode_Msg* pGCode_Msg,NC_Para* pNC_Para)
{
	int i;
	#if  USE_INTER_DEBUG
	   printf("装载处理1\r\n");
	#endif
	
	#if  USE_INTER_DEBUG
	   for(i=0;i<3;i++)
		{
			printf("pGCode_Msg->coor_value[%d]:%f\r\n",i,pGCode_Msg->coor_value[i]);
		}
	#endif

	
	for(i=0;i<3;i++)//插补终点更新
	{pNC_Para->coor_destinationValue[i]=(long int)(pGCode_Msg->coor_value[i]);}
	for(i=0;i<3;i++)//插补起点更新
	{pNC_Para->coor_originValue[i]=pNC_Para->coor_currentValue[i];}


	#if  USE_INTER_DEBUG
	   	printf("装载处理2\r\n");
	#endif	

	//G指令更新
	pNC_Para->G_instruct=pGCode_Msg->G_instruct;
	
	
	if((pNC_Para->G_instruct==0x02)||(pNC_Para->G_instruct==0x03))//G02 G03
	{
			//圆弧半径更新,和优弧劣弧模式humode
			if(pGCode_Msg->O_R>=0)//劣弧<180
			{
				pNC_Para->O_R=pGCode_Msg->O_R;
				pNC_Para->humode=0;
			}
			else
			{
				pNC_Para->O_R=-pGCode_Msg->O_R;
				pNC_Para->humode=1;
			}

	#if  USE_INTER_DEBUG
			printf("计算圆心\r\n");
	#endif	
			
			//更新圆心值
			GetArcO1(pNC_Para);
			
	#if  USE_INTER_DEBUG
			printf("装载计算O_x:%f  O_Y:%f \r\n",pNC_Para->O_X,pNC_Para->O_Y);
	#endif	

	}
	//更新步数
	pNC_Para->steps=GetSteps1(pNC_Para);
	
	//偏差清零
	pNC_Para->f=0;
	
}


//函数：line_interpolation
//功能：单步进行逐点比较法直线插补
//输入：插补信息结构体地址pNC_Para
void line_interpolation(NC_Para* pNC_Para){
	

	
	if(pNC_Para->steps>0)

   {
			pNC_Para->steps=pNC_Para->steps-1;//步数自减1

	
   switch(quad_test2( pNC_Para->coor_destinationValue[0]-pNC_Para->coor_originValue[0],
											pNC_Para->coor_destinationValue[1]-pNC_Para->coor_originValue[1],
											pNC_Para->G_instruct)
				)
		{
			
			case 1: 
				if((pNC_Para->f)>=0)
				{ 
					pNC_Para->coor_currentValue[0]+=1;//X++
					run_pos1(XAXIS);
			
					pNC_Para->f=pNC_Para->f-(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_originValue[1]);
				}
				else
				{
					//y++; 
					pNC_Para->coor_currentValue[1]+=1;
					run_pos1(YAXIS);
					pNC_Para->f=pNC_Para->f+(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_originValue[0]);


				}
				break;	

			case 2:
				//if(pNC_Para->f>=0)
				if(pNC_Para->f<0)
				{
					pNC_Para->coor_currentValue[0]--;
					run_neg1(XAXIS); 
					pNC_Para->f=pNC_Para->f+(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_originValue[1]);
				}
				else
				{
					pNC_Para->coor_currentValue[1]++;
					run_pos1(YAXIS);
					pNC_Para->f=pNC_Para->f+(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_originValue[0]);
				
				}								
				break;

			case 3:
				if(pNC_Para->f>0)
				{
				
					pNC_Para->coor_currentValue[1]--;
					run_neg1(YAXIS); 
					
					pNC_Para->f=pNC_Para->f+(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_originValue[0]);
				}
				else
				{
					
					pNC_Para->coor_currentValue[0]--;
					run_neg1(XAXIS); 
					
					pNC_Para->f=pNC_Para->f-(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_originValue[1]);
				}
				break;

			case 4:
				if(pNC_Para->f>=0)
				{
					
					pNC_Para->coor_currentValue[1]--;
					run_neg1(YAXIS); 
					pNC_Para->f=pNC_Para->f-(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_originValue[0]);

				}
				else
				{
					
					pNC_Para->coor_currentValue[0]++;
					run_pos1(XAXIS);
					pNC_Para->f=pNC_Para->f-(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_originValue[1]);
				}
				break;
				
				
			default:
				printf("%s errro!",__func__);
			break;
 		}

   }
}


//函数：circ_interpolation
//功能：单步进行逐点比较法逆圆弧插补
//输入：插补信息结构体地址pNC_Para
void circ_interpolation(NC_Para* pNC_Para)
{

	#if  USE_INTER_DEBUG
			printf("进入圆弧算法 ");
	#endif	
		if(pNC_Para->steps>0)
   {
			pNC_Para->steps=pNC_Para->steps-1;//步数自减1
		 
	#if  USE_INTER_DEBUG
		 printf("圆弧象限：%d  ",quad_test2( pNC_Para->coor_currentValue[0]-pNC_Para->O_X,
																					pNC_Para->coor_currentValue[1]-pNC_Para->O_Y,
																					pNC_Para->G_instruct)
						);
	#endif		

		switch(quad_test2( pNC_Para->coor_currentValue[0]-pNC_Para->O_X,
											pNC_Para->coor_currentValue[1]-pNC_Para->O_Y,
											pNC_Para->G_instruct)
					)
			{
				
				case 1:
					if(pNC_Para->f>=0)
					{
					
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]--;
						
						run_neg1(XAXIS); 
					}
					else
					{
						
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]++;
						run_pos1(YAXIS);
						
					}
					break;	

				case 2:
					if(pNC_Para->f>=0)
					{
						////run_neg(2);//ymotor a step back
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]--;
						run_neg1(YAXIS); 
						
					}
					else
					{
						////run_neg(1);//xmotor a step back
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]--;
						run_neg1(XAXIS); 
					}								
					break;

				case 3:
					if(pNC_Para->f>=0)
					{
						//run_pos1(1);//xmotor a step forward
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]++;
						run_pos1(XAXIS);
					}
					else
					{
						////run_neg(2);//ymotor a step back
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]--;
						run_neg1(YAXIS);
					}
					break;
					
				case 4:
					if(pNC_Para->f>=0)
					{
						////run_pos(2);//ymotor a step forward
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]++;
						run_pos1(YAXIS);
					}
					else
					{
						////run_pos(1);//xmotor a step forward
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]++;
						run_pos1(XAXIS);
					}								
					break;
			}
			//updataCurrentXY();//刷新LCD上的显示
		
//到达终点结束			
		if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])==0&&(int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])==0)){return;}
		//	delay_ms(2000);
		}
		
		
// 		if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])!=0)||((int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])!=0))
// 			{
// 			//步数计算误差补偿
// 			pNC_Para->steps=0;
// 			//最后一步误差处理
// 			switch((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0]))
// 			{
// 					case 0: break;
// 					case 1: run_pos1(XAXIS); 
// 						pNC_Para->coor_currentValue[0]++; break;
// 					case -1: run_neg1(XAXIS); 
// 						pNC_Para->coor_currentValue[0]--;break;
// 					default :break;
// 			}
// 					switch((int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1]))
// 			{
// 					case 0: break;
// 					case 1: run_pos1(YAXIS); 
// 						pNC_Para->coor_currentValue[1]++; break;
// 					case -1: run_neg1(YAXIS);
// 						pNC_Para->coor_currentValue[1]--;break;
// 					default :;break;
// 			}
// 			

// 	}
// 		else return;

}


//函数：circr_interpolation
//功能：单步进行逐点比较法顺圆弧插补
//输入：插补信息结构体地址pNC_Para
void circr_interpolation(NC_Para* pNC_Para)
{

		if(pNC_Para->steps>0)
   {
			pNC_Para->steps=pNC_Para->steps-1;//步数自减1
		switch(quad_test2( pNC_Para->coor_currentValue[0]-pNC_Para->O_X,
											pNC_Para->coor_currentValue[1]-pNC_Para->O_Y,
											pNC_Para->G_instruct)
					)
			{
				case 1:
					if(pNC_Para->f<0)
					{
						////run_neg(1);//xmotor a step back
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]++;
						run_pos1(XAXIS);
						
					}
					else
					{
						////run_neg(2);//ymotor a step back
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]--;
						run_neg1(YAXIS);
						
					}
					break;	

				case 2:
					if(pNC_Para->f<0)
					{
						//ymotor a step forward
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]++;
						run_pos1(YAXIS);
					}
					else
					{
						//xmotor a step forward
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]++;
						run_pos1(XAXIS);
						
					}								
					break;

				case 3:
					if(pNC_Para->f<0)
					{
						//xmotor a step back
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]--;
						run_neg1(XAXIS);
						
					}
					else
					{
					//ymotor a step forward
						pNC_Para->f=pNC_Para->f+2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]++;
						run_pos1(YAXIS);
						
					}
					break;
					
				case 4:
					if(pNC_Para->f<0)
					{
						//ymotor a step back
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[1]-pNC_Para->O_Y)+1;
						pNC_Para->coor_currentValue[1]--;
						run_neg1(YAXIS);
						
					}
					else
					{
						//xmotor a step back
						pNC_Para->f=pNC_Para->f-2*(pNC_Para->coor_currentValue[0]-pNC_Para->O_X)+1;
						pNC_Para->coor_currentValue[0]--;
						run_neg1(XAXIS);
						
					}								
					break;

			}
			//updataCurrentXY();//刷新LCD上的显示
			//到达终点结束			
		//if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])==0&&(int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])==0)){return;}
		}
		
// 			if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])!=0||(int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])!=0))
// 				{
// 			//步数计算误差补偿
// 			int steps_compensate=0;	
// 			steps_compensate=(int )fabs(pNC_Para->coor_currentValue[0]-pNC_Para->coor_destinationValue[0])+(int )fabs(pNC_Para->coor_currentValue[1]-destination[0]);
// 			pNC_Para->steps=steps_compensate;
// 			//current_position[0]=pNC_Para->coor_currentValue[0];
// 			//current_position[1]=pNC_Para->coor_currentValue[1];
// 			//	mode='n';
// 			//circr_inter();
// 				pNC_Para->coor_originValue[0]=pNC_Para->coor_currentValue[0];
// 				pNC_Para->coor_originValue[1]=pNC_Para->coor_currentValue[1];
// 			circr_interpolation( pNC_Para);	
// 				
// 			pNC_Para->steps=0;
// 			//最后一步误差处理
// 			switch((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0]))
// 			{
// 					case 0: break;
// 					case 1: run_pos1(XAXIS); 
// 						pNC_Para->coor_currentValue[0]++; break;
// 					case -1: run_neg1(XAXIS); 
// 						pNC_Para->coor_currentValue[0]--;break;
// 					default :;break;
// 			}
// 					switch((int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1]))
// 			{
// 					case 0: break;
// 					case 1: run_pos1(YAXIS);
// 						pNC_Para->coor_currentValue[1]++; break;
// 					case -1: run_neg1(YAXIS); 
// 						pNC_Para->coor_currentValue[1]--;break;
// 					default :break;
// 			}
// 			
// 			//updataCurrentXY();//刷新LCD上的显示
// 	}
}






