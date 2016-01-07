#include "KEY.h"
#include "delay.h"
void KEY_Init(void){
	GPIO_InitTypeDef GPIO_InitTypeStruc;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruc);
	
	GPIO_InitTypeStruc.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitTypeStruc.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(GPIOA,&GPIO_InitTypeStruc);
	
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES��KEY0����
//KEY1_PRES��KEY1����
//WKUP_PRES��WK_UP���� 
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode){
	static u8 key_up=1;
	if(mode) key_up=1;
	
	if(key_up&&((KEY1==0)||(KEY0==0)||(WK_UP==1))){
		delay_ms(10);//ȥ��
		key_up=0;
		if(KEY0==0) return KEY0_PRES;
		else if(KEY1==0) return KEY1_PRES;
		else if(WK_UP==1) return WKUP_PRES;
		
	}else if((KEY0==1)&&(KEY1==1)&&(WK_UP==0)) key_up=1;
	return 0;
	
}

