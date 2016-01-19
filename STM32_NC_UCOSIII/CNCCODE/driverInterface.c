#include "driverInterface.h"
#include "interpolation.h"
#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "usart.h"
#include "led.h"
#include "malloc.h"
#include "string.h"

//����ѡ�� 1����ʹ�ô���������� 0��ʹ��
#define USE_DRIVER_DEBUG (0)

#if  USE_DRIVER_DEBUG

#endif


#define INTERVAL 5  //LCD��ͼ�ĵ�λ
extern u16 POINT_COLOR_LAST;//��ʱ��Ż�����ɫ
extern u8 ZOOM;


#define RUN_POS 1   //��ת
#define RUN_NEG 0		//��ת

#define XAXIS  1  //����תʱָ����
#define YAXIS  2

//��B2..7����
#define X_DIR_PIN PAout(8)//X Y Z ������ķ����ź����Ŷ���
#define Y_DIR_PIN PAout(11)
#define Z_DIR_PIN PAout(12)

#define X_PUL_PIN PAout(4)//X Y Z ������������ź����Ŷ���
#define Y_PUL_PIN PCout(4)
#define Z_PUL_PIN PCout(5)

//��λ����λ��
#define X_ORG PDin(2)//X Y Z �����������ź����Ŷ���
#define Y_ORG PCin(11)
#define Z_ORG PCin(12)

char IS_XOrg=0;
char IS_YOrg=0;
char IS_ZOrg=1;
//ԭ���б��־
//extern char IS_XOrg;
//extern char IS_YOrg;
//extern char IS_ZOrg;
extern u16 Temp_X,Temp_Y; //��������ƽ��
extern NC_Para *pNC_Para;

//���Խӿں���
void testGPIO(void)
{
	while(1)
	{
		Y_PUL_PIN=0;
		X_PUL_PIN=0;
		delay_ms(1000);
		Y_PUL_PIN=1;
		X_PUL_PIN=1;

		
	}
}
//������driver_GPIOInit
//���ܣ����ڵ�������Ľӿڳ�ʼ��
//���壺PA.8 PA.11 PA.12 �ֱ�Ϊ X Y Z����������ķ����ź�  
//------PA.4 PC. 4 PC. 5 �ֱ�Ϊ X Y Z����������������ź� 
void driver_GPIOInit1(void ){
	
//	GPIO_InitTypeDef GPIO_InitTypeStruc;
//	NVIC_InitTypeDef NVIC_InitTypeStruc;
//	EXTI_InitTypeDef EXTI_InitTypeStruc;
//	
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
//	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
//	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOC,&GPIO_InitTypeStruc);
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;
//	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
//	
//	GPIO_Init(GPIOA,&GPIO_InitTypeStruc);
//	
//	//����ӿ�
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
//	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
//	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_IPU;
//	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC,&GPIO_InitTypeStruc);
//	
//	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_2;
//	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_IPU;
//	GPIO_InitTypeStruc.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_Init(GPIOD,&GPIO_InitTypeStruc);
//	
//	EXTI_InitTypeStruc.EXTI_Line=EXTI_Line2;
//	EXTI_InitTypeStruc.EXTI_Mode=EXTI_Mode_Interrupt;
//	EXTI_InitTypeStruc.EXTI_Trigger=EXTI_Trigger_Falling;
//	EXTI_InitTypeStruc.EXTI_LineCmd=ENABLE;
//	EXTI_Init(&EXTI_InitTypeStruc);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource2);
//	
//	EXTI_InitTypeStruc.EXTI_Line=EXTI_Line11;
//	EXTI_InitTypeStruc.EXTI_Mode=EXTI_Mode_Interrupt;
//	EXTI_InitTypeStruc.EXTI_Trigger=EXTI_Trigger_Falling;
//	EXTI_InitTypeStruc.EXTI_LineCmd=ENABLE;
//	EXTI_Init(&EXTI_InitTypeStruc);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
//	
//	EXTI_InitTypeStruc.EXTI_Line=EXTI_Line12;
//	EXTI_InitTypeStruc.EXTI_Mode=EXTI_Mode_Interrupt;
//	EXTI_InitTypeStruc.EXTI_Trigger=EXTI_Trigger_Falling;
//	EXTI_InitTypeStruc.EXTI_LineCmd=ENABLE;
//	EXTI_Init(&EXTI_InitTypeStruc);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource12);
//	
//	NVIC_InitTypeStruc.NVIC_IRQChannel=EXTI2_IRQn;
//	NVIC_InitTypeStruc.NVIC_IRQChannelPreemptionPriority=0x02;
//	NVIC_InitTypeStruc.NVIC_IRQChannelSubPriority=0x00;
//	NVIC_InitTypeStruc.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_Init(&NVIC_InitTypeStruc);

//	NVIC_InitTypeStruc.NVIC_IRQChannel=EXTI15_10_IRQn;
//	NVIC_InitTypeStruc.NVIC_IRQChannelPreemptionPriority=0x02;
//	NVIC_InitTypeStruc.NVIC_IRQChannelSubPriority=0x00;
//	NVIC_InitTypeStruc.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_Init(&NVIC_InitTypeStruc);
	
	//testGPIO( );
}

//������
//

void run_pos1(u8 ii)
 {
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

	void run_neg1(u8 jj)
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

//����3��Ϊ�жϴ��������ֱ��Ӧ������λ���ص���Ӧ
 void EXTI2_IRQHandler(void){
	 printf("X�����ж�\r\n");
 	if(EXTI_GetITStatus(EXTI_Line2)!=RESET){
		
		delay_ms(10);
		if(X_ORG==0)//������������
		{
			IS_XOrg=1;
			printf("X���ص���\r\n");
		}

 		
 		EXTI_ClearITPendingBit(EXTI_Line2);
 	}

 }

 void EXTI15_10_IRQHandler(void){
	 printf("YZ�����ж�\r\n");
 	if(EXTI_GetITStatus(EXTI_Line11)!=RESET){
		delay_ms(10);
		if(Y_ORG==0)//������������
 		{
			IS_YOrg=1;
			printf("Y���ص���\r\n");
		}
 		
 		EXTI_ClearITPendingBit(EXTI_Line11);
 	}
	
	 	if(EXTI_GetITStatus(EXTI_Line12)!=RESET){
		delay_ms(10);
		if(Z_ORG==0)//������������
 		{
			IS_ZOrg=1;
			printf("Z���ص���\r\n");
		}
 		
 		EXTI_ClearITPendingBit(EXTI_Line12);
 	}

 }

void axisToZero1(void)
	{
	while((!IS_XOrg)||(!IS_YOrg)||(!IS_YOrg))//���᲻��ԭ��
	{
		if(!IS_XOrg)
		{
			run_neg1(XAXIS);
			printf("X goto Zero \r\n");
		}
		
			if(!IS_YOrg)
		{
			run_neg1(YAXIS);
			printf("Y goto Zero \r\n");
		}
		
				if(!IS_ZOrg)
		{
			//Z��תһ������
			//run_neg(ZAXIS);
			printf("Z goto Zero \r\n");
		}
		
		delay_ms(50);//��������
	}

}
//������updataShow
//���ܣ�ˢ����ʾLCD����ʾ�Ĳ���,���ƹ켣��
//���룺�岹�����ṹ��ָ��pNC_Para��ˢ��ģʽshowMode��0ˢ������ҳ������ 1ˢ��X Y Step���� 
int displayRate=0;
void updataShow(NC_Para *pNC_Para,char showMode){
	
	int x=pNC_Para->coor_currentValue[0],y=pNC_Para->coor_currentValue[1];//����ֵת��Ϊ������ʾ
	int k[2]={1,1};//LCD�����Ŵ���ϵ��
//���ƹ켣��
	displayRate++;
if(displayRate==(1*ZOOM)){ displayRate=0;
	LCD_DrawPoint(lcddev.width/2+x/ZOOM, lcddev.width-(lcddev.width/2+y/ZOOM));

	}
		
	//ʵʱ������ʾ
 	if(x<0)
 	{ 
 		k[0]=-1;
 				LCD_ShowChar(Temp_X+8*3,Temp_Y,'-',16,0);//��ʾ����
 	}
 	else
 	{
 		k[0]=1;
 		LCD_ShowChar(Temp_X+8*3,Temp_Y,' ',16,0);//����ʾ����
 	}
 		if(y<0)
 	{ 
 		k[1]=-1;
 				LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,'-',16,0);//��ʾ����
 	}
 		else
 	{
 		k[1]=1;
 		LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,' ',16,0);//����ʾ����
 	}
	//ʵ�����귢�͵�����
	printfPosition(x,y,0,1,0,500);
 		
 	//ˢ������x y	
 	LCD_ShowNum(Temp_X+8*4, Temp_Y,x*k[0],5,16);
 	LCD_ShowNum(Temp_X+8*15, Temp_Y,y*k[1],5,16);
 	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,pNC_Para->steps,6,16);
	
	if(showMode==0)
	{

		u8 i=0;
	 int k0[2]={1,1};
	 int ke[2]={1,1};
		for(i=0;i<2;i++){
			if(pNC_Para->coor_originValue[i]<0) 
			{	
				k0[i]=-1;
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,'-',16,0);//��ʾ����
			}else
			{
				k0[i]=1;
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,' ',16,0);//��ʾ����
			}
			
				if(pNC_Para->coor_destinationValue[i]<0) 
			{	
				ke[i]=-1;
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,'-',16,0);//��ʾ����
			}else
			{
				ke[i]=1;
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,' ',16,0);//��ʾ����
			}
			
	}
	 //ˢ�²���x0 y0 xe ye
	 LCD_ShowNum(Temp_X+8*4, Temp_Y+16,(pNC_Para->coor_originValue[0]*k0[0]),5,16);
	LCD_ShowNum(Temp_X+8*15, Temp_Y+16,(pNC_Para->coor_originValue[1]*k0[1]),5,16);
		LCD_ShowNum(Temp_X+8*4, Temp_Y+16*2,(pNC_Para->coor_destinationValue[0]*ke[0]),5,16);
	LCD_ShowNum(Temp_X+8*15, Temp_Y+16*2,(pNC_Para->coor_destinationValue[1]*ke[1]),5,16);
	
	 LCD_ShowNum(Temp_X+8*3, Temp_Y+16*3,1,3,16);
	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,pNC_Para->steps,6,16);

	}
}

//ˢ��ʵʱ����
void LCD_updateCurrentPosition(NC_Para *pNC_Para)
{
		int k[2]={1,1};//LCD�����Ŵ���ϵ��
		int x=pNC_Para->coor_currentValue[0],y=pNC_Para->coor_currentValue[1];//����ֵת��Ϊ������ʾ
			//ʵʱ������ʾ
 	if(x<0)
 	{ 
 		k[0]=-1;
 				LCD_ShowChar(Temp_X+8*3,Temp_Y,'-',16,0);//��ʾ����
 	}
 	else
 	{
 		k[0]=1;
 		LCD_ShowChar(Temp_X+8*3,Temp_Y,' ',16,0);//����ʾ����
 	}
 		if(y<0)
 	{ 
 		k[1]=-1;
 				LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,'-',16,0);//��ʾ����
 	}
 		else
 	{
 		k[1]=1;
 		LCD_ShowChar(Temp_X+8*3+8*11,Temp_Y,' ',16,0);//����ʾ����
 	}
 		
 	//ˢ������x y	
 	LCD_ShowNum(Temp_X+8*4, Temp_Y,x*k[0],5,16);
 	LCD_ShowNum(Temp_X+8*15, Temp_Y,y*k[1],5,16);
 	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,pNC_Para->steps,6,16);
}


void LCD_ShowFrame1(void){
	//���ô�����ɫ
	LCD_Clear(BLACK);
	LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//���ָ����ɫ
	LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//���ָ����ɫ
	
	POINT_COLOR_LAST=POINT_COLOR;//���滭����ɫ
	POINT_COLOR=BLACK;//�л�������ɫ
	//���
	LCD_DrawRectangle(INTERVAL/5, INTERVAL/5, lcddev.width-1-INTERVAL/5, lcddev.height-1-INTERVAL/5);
	//�ϱ߿�
	LCD_DrawRectangle(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL);
	//�±߿�
	LCD_DrawRectangle(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL);
	
	//��������
	LCD_DrawLine(INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2, lcddev.width/2);
	//��X���ͷ
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2-INTERVAL*0.6);
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2+INTERVAL*0.6);
	//��������
	LCD_DrawLine(lcddev.width/2, INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2);
	//Y���ͷ
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2-INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2+INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	
	POINT_COLOR=POINT_COLOR_LAST;//�ָ�������ɫ
	
	//�ı���Ϣ
	Temp_X=INTERVAL+3;
	Temp_Y=lcddev.width+3;
	LCD_ShowString(Temp_X, Temp_Y,8*9,16,16,"X :");
	LCD_ShowString(Temp_X+8*11, Temp_Y,6*9,16,16,"Y :");
	LCD_ShowString(Temp_X, Temp_Y+16,8*9,16,16,"X0:");
	LCD_ShowString(Temp_X+8*11, Temp_Y+16,8*9,16,16,"Y0:");
	LCD_ShowString(Temp_X, Temp_Y+16*2,8*9,16,16,"Xe:");
	LCD_ShowString(Temp_X+8*11, Temp_Y+16*2,8*9,16,16,"Ye:");
	LCD_ShowString(Temp_X, Temp_Y+16*3,8*9,16,16,"G :");
	LCD_ShowString(Temp_X+8*8, Temp_Y+16*3,8*9,16,16,"steps:");

}

void tim_Init(uint16_t  TIM_Peri,uint16_t  TIM_Pres ){

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeStruc;
	NVIC_InitTypeDef NVIC_InitTypeStruc;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitTypeStruc.TIM_Prescaler=TIM_Pres;
	TIM_TimeBaseInitTypeStruc.TIM_Period=TIM_Peri;
	TIM_TimeBaseInitTypeStruc.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitTypeStruc.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitTypeStruc);
	
	
	NVIC_InitTypeStruc.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitTypeStruc.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitTypeStruc.NVIC_IRQChannelSubPriority=0;
	NVIC_InitTypeStruc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitTypeStruc);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	//TIM_Cmd(TIM3,ENABLE);
	
}


void interpolationChangeSpeed(unsigned int period)//period*100us
{
	tim_Init(period,7199 );
}

//�򿪶�ʱ��ʼ�岹����
void interpolationTaskCreat()
{
	//�򿪶�ʱ��
#if  USE_DRIVER_DEBUG
	printf("�򿪶�ʱ��\r\n");
#endif

	
	pNC_Para->IS_INTERPOLATINO=1;
	TIM_Cmd(TIM3,ENABLE);
	
}

//�رն�ʱ��������ʱ������
void interpolationTaskDelet()
{
	//�򿪶�ʱ��
	TIM_Cmd(TIM3,DISABLE);
	pNC_Para->IS_INTERPOLATINO=0;
#if  USE_DRIVER_DEBUG
	printf("�رն�ʱ��\r\n");
#endif

}

//��ʱ������
void TIM3_IRQHandler( void ){

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET){
	
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
		LED0=!LED0;
		//��ʾ��LCD,ʹ����ʾ������ʱ��
		//��ʾģʽ1��ֻ��ʵʱ����ˢ��
		updataShow(pNC_Para,1);
		
		//ֻˢ��ʵʱ����
		//LCD_updateCurrentPosition(pNC_Para);
		
		
		if(pNC_Para->steps>0)
		{
			//�˴���Ӵ����޸���һ�ζ�ʱ���ڡ��ı�岹�ٶ�

			//�岹
#if  USE_DRIVER_DEBUG
			printf("ѡ��岹�㷨 G:%d/r/n",pNC_Para->G_instruct);
#endif

			switch (pNC_Para->G_instruct)
			{
				case 0x01://ֱ�߲岹
					line_interpolation( pNC_Para);
#if  USE_DRIVER_DEBUG
					printf("line:%ld,%ld\r\n",pNC_Para->coor_currentValue[0],pNC_Para->coor_currentValue[1]);
#endif				
					break;
				
				case 0x02://��Բ���岹
					circ_interpolation(pNC_Para);				
#if  USE_DRIVER_DEBUG
					printf("circ:%ld,%ld\r\n",pNC_Para->coor_currentValue[0],pNC_Para->coor_currentValue[1]);
#endif	
					break;
				
				case 0x03://˳Բ���岹
					circr_interpolation(pNC_Para);
#if  USE_DRIVER_DEBUG
					printf("circn:%ld,%ld\r\n",pNC_Para->coor_currentValue[0],pNC_Para->coor_currentValue[1]);
#endif
					break;
			}
		}
		else {
			//�岹����������رն�ʱ��
			interpolationTaskDelet();//�رն�ʱ�� 
		}
		
	}
}

//���ڷ���ʵʱ��������
//Э���룺>>10#X#Y#Z#T#F#S
//���ݰ���#�Ÿ���

void printfPosition(int x,int y,int z,int t,int f,int s)
{
		char * msg = (char *)malloc(40);
		mymemset(msg,0,strlen(msg));
		sprintf(msg,">>10#%d#%d#%d#%d#%d#%d",x,y,z,t,f,s);
		//printf("position:%s \r\n",msg);
	
		printf("%s \r\n",msg);
		delay_ms(10);
		free(msg);
}




