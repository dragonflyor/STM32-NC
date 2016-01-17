#ifndef  __INTERPOLATION_H
#define  __INTERPOLATION_H

#include "sys.h"


//���ͺ��G�������
//�ռ��С:4*3+4*1+2*1+1*1+1*1=20byte;
//�ַ�������ͺ󱣴�
typedef struct {
	
	double coor_value[3]; //G���� X Y Z
	double O_R;//Բ���岹�뾶
	int F;//�岹�ٶ�
	char G_instruct;//Gָ�� G00 G01 G02 G03����
	char M_instruct;//M����ָ��
}GCode_Msg;



//����岹�����еĲ���
//�ռ��С��4*3+4*3+4*3+4*1+2*1+1*1+1*1+2*1+4*2=54Byte
typedef struct{
	 long int  coor_currentValue[3]; //��ǰ����
	 long int  coor_originValue[3];	//�������
	 long int coor_destinationValue[3];//�յ�����
	//float O_R;//Բ���岹�뾶
	long int O_R;//Բ���岹�뾶
	int F;//�岹�ٶ�
	char G_instruct;//Gָ�� G00 G01 G02 G03����
	long int f;//��ǰλ��������λ�õ�ƫ��
	//unsigned int steps;//ʣ�ಽ��
	long int steps;//ʣ�ಽ��
	//float O_X,O_Y;//Բ�ĺ�������
	double O_X,O_Y;//Բ�ĺ�������
	char humode;//Բ���岹ģʽ0 �ӻ� 1�Ż�
	char IS_INTERPOLATINO;//��ʱ���Ƿ����ڽ��в岹���̣�0 ���� 1 ��
} NC_Para;

void line_interpolation(NC_Para* pNC_Para);
void circ_interpolation(NC_Para* pNC_Para);
void circr_interpolation(NC_Para* pNC_Para);

void decode(char* strcmdbuffer);
void prepare_interpolation(GCode_Msg* pGCode_Msg,NC_Para* pNC_Para);
void updataShow(NC_Para *pNC_Para,char showMode);

void interpolationTaskCreat(void);
void interpolationTaskDelet(void);
void GetArcO1(NC_Para* pNC_Para);


#endif

