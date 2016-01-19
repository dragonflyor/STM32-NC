#include "driverInterface.h"
#include "interpolation.h"
#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "usart.h"
#include "led.h"
#include "malloc.h"
#include "string.h"

//调试选择 1驱动使用串口输出调试 0不使用
#define USE_DRIVER_DEBUG (0)

#if  USE_DRIVER_DEBUG

#endif


#define INTERVAL 5  //LCD绘图的单位
extern u16 POINT_COLOR_LAST;//临时存放画笔颜色
extern u8 ZOOM;


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
#define X_ORG PDin(2)//X Y Z 三个轴的起点信号引脚定义
#define Y_ORG PCin(11)
#define Z_ORG PCin(12)

char IS_XOrg=0;
char IS_YOrg=0;
char IS_ZOrg=1;
//原点判别标志
//extern char IS_XOrg;
//extern char IS_YOrg;
//extern char IS_ZOrg;
extern u16 Temp_X,Temp_Y; //窗口坐标平移
extern NC_Para *pNC_Para;

//测试接口函数
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
//函数：driver_GPIOInit
//功能：关于电机驱动的接口初始化
//定义：PA.8 PA.11 PA.12 分别为 X Y Z电机驱动器的方向信号  
//------PA.4 PC. 4 PC. 5 分别为 X Y Z电机驱动器的脉冲信号 
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
//	//回零接口
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

//函数：
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

//下面3个为中断处理函数，分别对应三个限位开关的响应
 void EXTI2_IRQHandler(void){
	 printf("X进入中断\r\n");
 	if(EXTI_GetITStatus(EXTI_Line2)!=RESET){
		
		delay_ms(10);
		if(X_ORG==0)//消除抖动后检测
		{
			IS_XOrg=1;
			printf("X开关到零\r\n");
		}

 		
 		EXTI_ClearITPendingBit(EXTI_Line2);
 	}

 }

 void EXTI15_10_IRQHandler(void){
	 printf("YZ进入中断\r\n");
 	if(EXTI_GetITStatus(EXTI_Line11)!=RESET){
		delay_ms(10);
		if(Y_ORG==0)//消除抖动后检测
 		{
			IS_YOrg=1;
			printf("Y开关到零\r\n");
		}
 		
 		EXTI_ClearITPendingBit(EXTI_Line11);
 	}
	
	 	if(EXTI_GetITStatus(EXTI_Line12)!=RESET){
		delay_ms(10);
		if(Z_ORG==0)//消除抖动后检测
 		{
			IS_ZOrg=1;
			printf("Z开关到零\r\n");
		}
 		
 		EXTI_ClearITPendingBit(EXTI_Line12);
 	}

 }

void axisToZero1(void)
	{
	while((!IS_XOrg)||(!IS_YOrg)||(!IS_YOrg))//有轴不在原点
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
			//Z反转一个脉冲
			//run_neg(ZAXIS);
			printf("Z goto Zero \r\n");
		}
		
		delay_ms(50);//脉冲周期
	}

}
//函数：updataShow
//功能：刷新显示LCD中显示的参数,绘制轨迹点
//输入：插补参数结构体指针pNC_Para，刷新模式showMode：0刷新整个页面数据 1刷新X Y Step数据 
int displayRate=0;
void updataShow(NC_Para *pNC_Para,char showMode){
	
	int x=pNC_Para->coor_currentValue[0],y=pNC_Para->coor_currentValue[1];//坐标值转换为整数显示
	int k[2]={1,1};//LCD正负号处理系数
//绘制轨迹点
	displayRate++;
if(displayRate==(1*ZOOM)){ displayRate=0;
	LCD_DrawPoint(lcddev.width/2+x/ZOOM, lcddev.width-(lcddev.width/2+y/ZOOM));

	}
		
	//实时坐标显示
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
	//实际坐标发送到串口
	printfPosition(x,y,0,1,0,500);
 		
 	//刷新坐标x y	
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
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,'-',16,0);//显示负号
			}else
			{
				k0[i]=1;
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,' ',16,0);//显示负号
			}
			
				if(pNC_Para->coor_destinationValue[i]<0) 
			{	
				ke[i]=-1;
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,'-',16,0);//显示负号
			}else
			{
				ke[i]=1;
				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,' ',16,0);//显示负号
			}
			
	}
	 //刷新参数x0 y0 xe ye
	 LCD_ShowNum(Temp_X+8*4, Temp_Y+16,(pNC_Para->coor_originValue[0]*k0[0]),5,16);
	LCD_ShowNum(Temp_X+8*15, Temp_Y+16,(pNC_Para->coor_originValue[1]*k0[1]),5,16);
		LCD_ShowNum(Temp_X+8*4, Temp_Y+16*2,(pNC_Para->coor_destinationValue[0]*ke[0]),5,16);
	LCD_ShowNum(Temp_X+8*15, Temp_Y+16*2,(pNC_Para->coor_destinationValue[1]*ke[1]),5,16);
	
	 LCD_ShowNum(Temp_X+8*3, Temp_Y+16*3,1,3,16);
	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,pNC_Para->steps,6,16);

	}
}

//刷新实时坐标
void LCD_updateCurrentPosition(NC_Para *pNC_Para)
{
		int k[2]={1,1};//LCD正负号处理系数
		int x=pNC_Para->coor_currentValue[0],y=pNC_Para->coor_currentValue[1];//坐标值转换为整数显示
			//实时坐标显示
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
 	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,pNC_Para->steps,6,16);
}


void LCD_ShowFrame1(void){
	//设置窗口颜色
	LCD_Clear(BLACK);
	LCD_Fill(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL,YELLOW);//填充指定颜色
	LCD_Fill(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL,WHITE);//填充指定颜色
	
	POINT_COLOR_LAST=POINT_COLOR;//保存画笔颜色
	POINT_COLOR=BLACK;//切换画笔颜色
	//外框
	LCD_DrawRectangle(INTERVAL/5, INTERVAL/5, lcddev.width-1-INTERVAL/5, lcddev.height-1-INTERVAL/5);
	//上边框
	LCD_DrawRectangle(INTERVAL, INTERVAL, lcddev.width-INTERVAL, lcddev.width-INTERVAL);
	//下边框
	LCD_DrawRectangle(INTERVAL, lcddev.width, lcddev.width-INTERVAL, lcddev.height-INTERVAL);
	
	//横坐标轴
	LCD_DrawLine(INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2, lcddev.width/2);
	//画X轴箭头
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2-INTERVAL*0.6);
	LCD_DrawLine(lcddev.width-INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*4, lcddev.width/2+INTERVAL*0.6);
	//纵坐标轴
	LCD_DrawLine(lcddev.width/2, INTERVAL*2, lcddev.width/2, lcddev.width-INTERVAL*2);
	//Y轴箭头
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2-INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	LCD_DrawLine(lcddev.width/2, INTERVAL*2,lcddev.width/2+INTERVAL*0.6, INTERVAL*2+INTERVAL*2);
	
	POINT_COLOR=POINT_COLOR_LAST;//恢复画笔颜色
	
	//文本信息
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

//打开定时开始插补驱动
void interpolationTaskCreat()
{
	//打开定时器
#if  USE_DRIVER_DEBUG
	printf("打开定时器\r\n");
#endif

	
	pNC_Para->IS_INTERPOLATINO=1;
	TIM_Cmd(TIM3,ENABLE);
	
}

//关闭定时器结束定时器驱动
void interpolationTaskDelet()
{
	//打开定时器
	TIM_Cmd(TIM3,DISABLE);
	pNC_Para->IS_INTERPOLATINO=0;
#if  USE_DRIVER_DEBUG
	printf("关闭定时器\r\n");
#endif

}

//定时器驱动
void TIM3_IRQHandler( void ){

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET){
	
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
		LED0=!LED0;
		//显示到LCD,使用显示会拖延时间
		//显示模式1，只有实时变量刷新
		updataShow(pNC_Para,1);
		
		//只刷新实时坐标
		//LCD_updateCurrentPosition(pNC_Para);
		
		
		if(pNC_Para->steps>0)
		{
			//此处添加代码修改下一次定时周期。改变插补速度

			//插补
#if  USE_DRIVER_DEBUG
			printf("选择插补算法 G:%d/r/n",pNC_Para->G_instruct);
#endif

			switch (pNC_Para->G_instruct)
			{
				case 0x01://直线插补
					line_interpolation( pNC_Para);
#if  USE_DRIVER_DEBUG
					printf("line:%ld,%ld\r\n",pNC_Para->coor_currentValue[0],pNC_Para->coor_currentValue[1]);
#endif				
					break;
				
				case 0x02://逆圆弧插补
					circ_interpolation(pNC_Para);				
#if  USE_DRIVER_DEBUG
					printf("circ:%ld,%ld\r\n",pNC_Para->coor_currentValue[0],pNC_Para->coor_currentValue[1]);
#endif	
					break;
				
				case 0x03://顺圆弧插补
					circr_interpolation(pNC_Para);
#if  USE_DRIVER_DEBUG
					printf("circn:%ld,%ld\r\n",pNC_Para->coor_currentValue[0],pNC_Para->coor_currentValue[1]);
#endif
					break;
			}
		}
		else {
			//插补任务结束，关闭定时器
			interpolationTaskDelet();//关闭定时器 
		}
		
	}
}

//串口发送实时坐标数据
//协议码：>>10#X#Y#Z#T#F#S
//数据包以#号隔开

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




