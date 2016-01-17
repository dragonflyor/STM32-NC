#include "interpolation.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "stdbool.h"
#include "usart.h"
#include "malloc.h"
#include "driverinterface.h"


char *strchr_pointer; //����ؼ��ֵĵ�ַָ��

//����ѡ�� 1�岹ʹ�ô���������� 0��ʹ��
#define USE_INTER_DEBUG (0)

#if  USE_INTER_DEBUG

#endif

#define NUM_AXIS 4
char axis_codes1[NUM_AXIS]= {'X', 'Y', 'Z', 'E'};//����G�����еĹؼ���

#define RUN_POS 1   //��ת
#define RUN_NEG 0		//��ת

#define XAXIS  1  //����תʱָ����
#define YAXIS  2


long int GetSteps1(NC_Para* pNC_Para);

extern char *strchr_pointer;
extern NC_Para* pNC_Para;
extern GCode_Msg* pGCode_Msg;
extern u8 ZOOM;

//������code_value
//���ܣ���ȡ�ַ�����ָ���ַ�ָ��strchr_pointer��������ֲ��������ڻ�ȡG�������ֵ
//���룺�ؼ��ֵ�ָ��strchr_pointer
//���أ�G�����йؼ��ֺ������ֵ
 float code_value1(char* strchr_pointer) 
{ 

	#if  USE_INTER_DEBUG
	printf("0------%s\r\n",(const char*)strchr_pointer);
	printf("1------%f\r\n",(float)strtod((const char*)(strchr_pointer+1),NULL));
	printf("2------%f\r\n",strtod("1.34569",NULL));
	#endif
	
	return ((float)strtod((strchr_pointer+1), NULL));
}

//������code_seen
//���ܣ������ַ���strcmdbuffer�еĹؼ���code
//���룺�ؼ���code
//���أ����ҵ��ؼ���1 ����0
 bool code_seen1(char code,char *strcmdbuffer)
	{



		strchr_pointer = strchr(strcmdbuffer, (int)code);
		
	#if  USE_INTER_DEBUG
		printf("code_seen�ҵ�%c %d\r\n",code,(strchr_pointer != NULL));
		printf("code_seen�ҵ�%s\r\n",(char*)(strchr_pointer) );
	#endif
	
		return ((strchr_pointer != NULL));	//Return True if a character was found
	}
	


//������get_coordinatesValue
//���ܣ������ַ���strcmdbuffer�е�����ֵ XYZ R
//���룺�ַ���strcmdbuffer �����ṹ��ָ��pNC_Para
//�����G������Ϣ�ṹ��ָ��pGCode_Msg
 void get_coordinatesValue1(char* strcmdbuffer,GCode_Msg* pGCode_Msg)
 {
	 int i;

	#if  USE_INTER_DEBUG
		printf("get_coordinatesValues����%s\r\n",(char*)(strcmdbuffer) );
	#endif
	
	
	 
	 //���ͱ�������ֵ XYZ
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
	//���ͱ���뾶 R
	 if(code_seen1('R',strcmdbuffer)){
			pGCode_Msg->O_R=code_value1(strchr_pointer)*ZOOM;
		}
		
 }
	
//������decode
//���ܣ���һ���ַ�ָ��������룬����õ�����Ϣ������pGCode_Msg�ṹ����
//���룺�ַ���strcmdbuffer 
//�����G������Ϣ�ṹ��ָ��pGCode_Msg
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
						get_coordinatesValue1(strcmdbuffer,pGCode_Msg);//��ȡ XYZR ����ֵ
						break;
				} 
				 case 2://G2 ��Բ���岹
				{
					 pGCode_Msg->G_instruct=0x02;
					get_coordinatesValue1(strcmdbuffer,pGCode_Msg);//��ȡ XYZR ����ֵ
					 break;
				}

				 case 3://G3 ˳Բ���岹
				{
						 pGCode_Msg->G_instruct=0x03;
						get_coordinatesValue1(strcmdbuffer,pGCode_Msg);//��ȡ XYZR ����ֵ
					 break;
				}		 
      }
   }
	 else if(code_seen1('M',strcmdbuffer))//����Mָ��
	 {
			pGCode_Msg->M_instruct=(int)code_value1(strchr_pointer);;
	 }
	 else if(code_seen1('F',strcmdbuffer))//�����ٶ�F
	 {
			 pGCode_Msg->F = code_value1(strchr_pointer);
	 }
}

// //������quad_test
// //���ܣ��岹��ʽG_instruct��x,y��������
// //���룺�����x,y �岹��ʽG_instruct
// //���أ�����ֵ
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


//������GetArcO
//���ܣ���ȡԲ���岹��Բ�����꣬װ�ص��岹�����ṹ��pNC_Para
//���룺�岹�����ṹ��ָ��pNC_Para
//������岹�����ṹ��ָ��pNC_Para�е�Բ��ֵO_X O_Y
//���ò�������GetSteps1 
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
	   printf("�����Բ�ģ�O_x1,O_y1��:(%f,%f)\r\n",pNC_Para->O_X,pNC_Para->O_Y);
#endif
		pNC_Para->O_X=X2;
		pNC_Para->O_Y=Y2;
 		Osteps2=GetSteps1(pNC_Para);
#if  USE_INTER_DEBUG
	   printf("�����Բ�ģ�O_x2,O_y2��:(%f,%f)\r\n",pNC_Para->O_X,pNC_Para->O_Y);
#endif
 	if(pNC_Para->humode==1)//�Ż�>180
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
 	else if(pNC_Para->humode==0)//�ӻ�<180
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
	//ȡ��
	//pNC_Para->O_X=(pNC_Para->O_X>=0)?(long int )(pNC_Para->O_X+0.5):(long int )(pNC_Para->O_X-0.5);
	//pNC_Para->O_Y=(pNC_Para->O_Y>=0)?(long int )(pNC_Para->O_Y+0.5):(long int )(pNC_Para->O_Y-0.5);
	pNC_Para->O_X=(pNC_Para->O_X>=0)?(pNC_Para->O_X):(pNC_Para->O_X);
	pNC_Para->O_Y=(pNC_Para->O_Y>=0)?(pNC_Para->O_Y):(pNC_Para->O_Y);
#if  USE_INTER_DEBUG
	   printf("�����Բ�ģ�O_x,O_y��:(%f,%f)\r\n",pNC_Para->O_X,pNC_Para->O_Y);
#endif
}

//������GetSteps
//���ܣ���岹��㵽�յ�Ĳ岹����
//���룺�岹�����ṹ��ָ��pNC_Para
//���أ�����
//��������Բ�ĺ���֮��ʹ�ã���Ҫ����quad_test�ж�������������
long int GetSteps1(NC_Para* pNC_Para)
{
	unsigned char a,b;
	long int steps1=0,steps2=0,steps;
	long int inR;
	long int O_x,O_y;
	

	
	inR=pNC_Para->O_R;
	O_x=pNC_Para->O_X;
	O_y=pNC_Para->O_Y;
	
	if(pNC_Para->G_instruct==0x02)//��Բ���岹
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
	
	if(pNC_Para->G_instruct==0x03)//�Բ���岹
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


//������prepare_interpolation
//���ܣ����岹��Ϣװ�ص�pNC_Para,Ϊ�岹׼���ò���
//���룺������Ϣ�ṹ���ַpGCode_Msg
//������岹��Ϣ�ṹ���ַpNC_Para
void prepare_interpolation(GCode_Msg* pGCode_Msg,NC_Para* pNC_Para)
{
	int i;
	#if  USE_INTER_DEBUG
	   printf("װ�ش���1\r\n");
	#endif
	
	#if  USE_INTER_DEBUG
	   for(i=0;i<3;i++)
		{
			printf("pGCode_Msg->coor_value[%d]:%f\r\n",i,pGCode_Msg->coor_value[i]);
		}
	#endif

	
	for(i=0;i<3;i++)//�岹�յ����
	{pNC_Para->coor_destinationValue[i]=(long int)(pGCode_Msg->coor_value[i]);}
	for(i=0;i<3;i++)//�岹������
	{pNC_Para->coor_originValue[i]=pNC_Para->coor_currentValue[i];}


	#if  USE_INTER_DEBUG
	   	printf("װ�ش���2\r\n");
	#endif	

	//Gָ�����
	pNC_Para->G_instruct=pGCode_Msg->G_instruct;
	
	
	if((pNC_Para->G_instruct==0x02)||(pNC_Para->G_instruct==0x03))//G02 G03
	{
			//Բ���뾶����,���Ż��ӻ�ģʽhumode
			if(pGCode_Msg->O_R>=0)//�ӻ�<180
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
			printf("����Բ��\r\n");
	#endif	
			
			//����Բ��ֵ
			GetArcO1(pNC_Para);
			
	#if  USE_INTER_DEBUG
			printf("װ�ؼ���O_x:%f  O_Y:%f \r\n",pNC_Para->O_X,pNC_Para->O_Y);
	#endif	

	}
	//���²���
	pNC_Para->steps=GetSteps1(pNC_Para);
	
	//ƫ������
	pNC_Para->f=0;
	
}


//������line_interpolation
//���ܣ������������ȽϷ�ֱ�߲岹
//���룺�岹��Ϣ�ṹ���ַpNC_Para
void line_interpolation(NC_Para* pNC_Para){
	

	
	if(pNC_Para->steps>0)

   {
			pNC_Para->steps=pNC_Para->steps-1;//�����Լ�1

	
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


//������circ_interpolation
//���ܣ������������ȽϷ���Բ���岹
//���룺�岹��Ϣ�ṹ���ַpNC_Para
void circ_interpolation(NC_Para* pNC_Para)
{

	#if  USE_INTER_DEBUG
			printf("����Բ���㷨 ");
	#endif	
		if(pNC_Para->steps>0)
   {
			pNC_Para->steps=pNC_Para->steps-1;//�����Լ�1
		 
	#if  USE_INTER_DEBUG
		 printf("Բ�����ޣ�%d  ",quad_test2( pNC_Para->coor_currentValue[0]-pNC_Para->O_X,
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
			//updataCurrentXY();//ˢ��LCD�ϵ���ʾ
		
//�����յ����			
		if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])==0&&(int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])==0)){return;}
		//	delay_ms(2000);
		}
		
		
// 		if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])!=0)||((int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])!=0))
// 			{
// 			//������������
// 			pNC_Para->steps=0;
// 			//���һ������
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


//������circr_interpolation
//���ܣ������������ȽϷ�˳Բ���岹
//���룺�岹��Ϣ�ṹ���ַpNC_Para
void circr_interpolation(NC_Para* pNC_Para)
{

		if(pNC_Para->steps>0)
   {
			pNC_Para->steps=pNC_Para->steps-1;//�����Լ�1
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
			//updataCurrentXY();//ˢ��LCD�ϵ���ʾ
			//�����յ����			
		//if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])==0&&(int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])==0)){return;}
		}
		
// 			if(((int)(pNC_Para->coor_destinationValue[0]-pNC_Para->coor_currentValue[0])!=0||(int)(pNC_Para->coor_destinationValue[1]-pNC_Para->coor_currentValue[1])!=0))
// 				{
// 			//������������
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
// 			//���һ������
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
// 			//updataCurrentXY();//ˢ��LCD�ϵ���ʾ
// 	}
}






