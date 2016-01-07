#ifndef __CNCCODE_H
#define __CNCCODE_H


//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "stdbool.h"
#include "lcd.h"
#include "getALineCodeFromSDTxt.h"


const char COLORS=9;
u16 layer_color[COLORS]={BRED,GRED,RED,MAGENTA ,GREEN ,YELLOW,BROWN,BRRED,GRAY  };
u8 colorCount=0;

const int NUM_AXIS = 4;
char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};
float current_position[NUM_AXIS] = {0.0, 0.0, 0.0, 0.0};
float destination[NUM_AXIS] = {0.0, 0.0, 0.0, 0.0};

int bufindr = 0;
int bufindw = 0;
int buflen = 0;
int i = 0;
char serial_char;
int serial_count = 0;
//bool comment_mode = false;
char *strchr_pointer; // just a pointer to find chars in the cmd string like X, Y, Z, E, etc 

// Manage heater variables. For a thermistor or AD595 thermocouple, raw values refer to the 
// reading from the analog pin. For a MAX6675 thermocouple, the raw value is the temperature in 0.25 
// degree increments (i.e. 100=25 deg). 

int target_raw = 0;
int current_raw = 0;
int target_bed_raw = 0;
int current_bed_raw = 0;
int tt = 0, bt = 0;

//20150108
int feedrate = 1500, next_feedrate, saved_feedrate;
bool relative_mode = false;  //Determines Absolute or Relative Coordinates
bool relative_mode_e = false;  //Determines Absolute or Relative E Codes while in Absolute Coordinates mode. E is always relative in Relative Coordinates mode.

bool axis_relative_modes[] = {false, false, false, false};
bool move_direction[NUM_AXIS];
float axis_diff[NUM_AXIS] = {0, 0, 0, 0};//??????????????,?(current_position[NUM_AXIS]-destination[NUM_AXIS])
unsigned long move_steps_to_take[NUM_AXIS];
float axis_steps_per_unit[] = {85.3333, 85.3333,2560,158.8308};
float d = 0;

float time_for_move;
long axis_interval[NUM_AXIS]; // for speed delay


int processCount=0;//?????????

long int steps=0;//全局步数
extern char mode;//全局模式
extern int x,y;//全局当前坐标
extern int f;
extern int inR;
extern char humode;

void get_coordinates(void);
bool code_seen(char code);
void linear_move(void);
void prepare_move(void);
void process_commands(void);
 void circ_move(void);

// float code_value() { return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL)); }
////  inline long code_value_long() { return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10)); }
//// bool code_seen(char code_string[]) { return (strstr(cmdbuffer[bufindr], code_string) != NULL); }  //Return True if the string was found
//// 

// bool code_seen(char code)
//  {
//    strchr_pointer = strchr(cmdbuffer[bufindr], (int)code);
//    return ((strchr_pointer != NULL));  //Return True if a character was found
//  }
// 
//	
//	
////decode 传一个结构体参数1
// void process_commands()
// {
//	 int i;
//	 
//	 printf("%-5d:process commands ing......--------------------------------\r\n",++processCount);
//   ////unsigned long codenum; //throw away variable
//   ////char *starpos = NULL;

//	 printf("Gpiontter:%d\r\n",code_seen('G'));

///*************************×××××××××××××××××××××××××××××××××××
//	 译码 ：包含获取坐标get_coordinates()，插补prepare_move()
//*************************×××××××××××××××××××××××××××××××××××××*/
//   if(code_seen('G'))//G??
//   {
//	   
//     switch((int)code_value())
//     {
//       case 0: // G0 -> G1
//       case 1: // G1
//		  mode='l';
//         get_coordinates(); // 得到X Y Z ，r的坐标
//         prepare_move();   //插补
//         //previous_millis_cmd = millis();

//         //ClearToSend();
//				
//		   printf("Line.\r\n");
//       //  return;
//         break;
//			 
//			 case 2://G2 逆圆弧插补
//				 mode='c';
//				get_coordinates(); // For X Y Z E R F  
//			 if(inR<0) {humode=1;inR=-inR;}//劣弧标志
//			 else humode=0;
//			 
//			 prepare_move();
//			 printf("CIRC.\r\n");
//			 break;
//			 
//			 	 case 3://G3 顺圆弧插补
//				 mode='n';
//				get_coordinates(); // For X Y Z E R F  
//			 if(inR<0) {humode=1;inR=-inR;}//劣弧标志
//			 else humode=0;
//			 
//			 prepare_move();
//			 break;
//				 
//	   case 21://G21 set units to millimeters
//			printf("set units to millimeters.\r\n");
//		   break;

//	   case 28: //G28 Home all Axis one at a time
//			printf("Home all Axis one at a time\r\n");

//			for(i=0; i < NUM_AXIS; i++) {
//				destination[i] = current_position[i];
//			}
//		   break;

//	   case 90: // G90 ?????
//		   printf("Absolute mode. \r\n");
//		   relative_mode = false;
//		   break;

//	   case 92: // G92 Changes the origin, so that the current position represents the given coordinates.
//		   for(i=0; i < NUM_AXIS; i++) {
//			   if(code_seen(axis_codes[i])) current_position[i] = code_value();  
//		   }
//		  //printf("??????????(%.3f,%.3f,%.3f)\n",current_position[0],current_position[1],current_position[2]);
//		  printf("Changes the origin\r\n");
//		   break;
//         
//     }
//   }
// 
//   else if(code_seen('M'))//M??
//   {
//     
//     switch( (int)code_value() ) 
//     {


//	 case 104: // M104
//		 if (code_seen('S')){
//				target_raw = code_value();
//		 } 
//			printf("%d C\r\n",target_raw); 
//		 break;
//	 case 82: //use absolute distances for extrusion
//		 printf("absolute distances for extrusion\r\n");
//		 break;
//	 case 83:
//		  printf("??????\r\n");
//		 break;
//	 case 84://M84  disable motors or set speed
//		 if(code_seen('S')){ 
//			 //stepper_inactive_time = code_value() * 1000; 
//		 }
//		 else{ //disable_x(); disable_y(); disable_z(); disable_e(); 
//		 printf("??????\r\n");
//		 }
//		 break;
//	 case 107: //M107 Fan Off
//		 printf("Fan Off\r\n");
//		 break;

//	 case 109: // M109 - Wait for extruder heater to reach target.
//		 if (code_seen('S')) 
//			 target_raw = code_value();
//			printf("Wait for extruder heater to %d\r\n",target_raw); 
//		 break;


//     
//    }
//     
//   }

//   else{//???????
//      printf("Unknown command:");
//      printf(cmdbuffer[bufindr]);
//	  printf("\n");
//   }
//   
//   //ClearToSend();
//       
// }


////参数结构体1
// void get_coordinates()
// {

//	int i;
// 	 for(i=0; i < NUM_AXIS; i++) {
//			if(code_seen(axis_codes[i])) {
//				destination[i]= (float)code_value() + (axis_relative_modes[i] || relative_mode)*current_position[i];
//				if(i==2) {POINT_COLOR=layer_color[colorCount++]; if(colorCount==COLORS) colorCount=0;}//改变不同层数的颜色
//			}
//			else destination[i] = current_position[i];                                                       //Are these else lines really needed?
//	 }

//		
//		//printf("The destination:(%.3f,%.3f,%.3f),",destination[0],destination[1],destination[2]);
//	 
//	 if(code_seen('R')){
//			inR=code_value();
//		 //printf("\r\nThe R:%d\r\n",inR);
//		}
//	 
//	 if(code_seen('F')) {
//		 next_feedrate = code_value();
//		 //printf("FeedSpeed:%d,",next_feedrate);
//	//	 //if(next_feedrate > 0.0) feedrate = next_feedrate;
// 	 }
//	 
// }

// 
//  void prepare_move()
// {

//	 if(mode=='l'){
//		linear_move(); // make the move
//	 }
//	 else if((mode=='c')||(mode=='n')){
//		 
//		 circ_move(); //圆弧插补
//	}

// }
// 
// 
// //单步直线插补结构体2
// extern u16 Temp_X,Temp_Y;
// //??????????????,??????????????????,????????????
//void linear_move() // make linear move with preset speeds and destinations, see G0 and G1
// {
//	u8 i=0;
//	 int k0[2]={1,1};
//	 int ke[2]={1,1};
//	//LCD模拟轨迹
////	 LCD_DrawLine(lcddev.width/2+current_position[0], lcddev.width/2+current_position[1], 
////								lcddev.width/2+destination[0], lcddev.width/2+destination[1]);
//	 
//	 //求steps
//	 //mode='l';
//	 f=0;
//	 
//	 
//	 //插补测试数据
//// 	 current_position[0]=0;
//// 	 current_position[1]=0;
//// 	 destination[0]=-10;
//// 	 destination[1]=-10;
//	 
//	 
//	 steps=GetSteps(mode);//直线步数
//	 //负数显示处理
//	for(i=0;i<2;i++){
//			if(current_position[i]<0) 
//			{	
//				k0[i]=-1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,'-',16,0);//显示负号
//			}else
//			{
//				k0[i]=1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,' ',16,0);//显示负号
//			}
//			
//				if(destination[i]<0) 
//			{	
//				ke[i]=-1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,'-',16,0);//显示负号
//			}else
//			{
//				ke[i]=1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,' ',16,0);//显示负号
//			}
//			
//	}
//	 //刷新参数x0 y0 xe ye
//	 LCD_ShowNum(Temp_X+8*4, Temp_Y+16,(current_position[0]*k0[0]),5,16);
//	LCD_ShowNum(Temp_X+8*15, Temp_Y+16,(current_position[1]*k0[1]),5,16);
//		LCD_ShowNum(Temp_X+8*4, Temp_Y+16*2,(destination[0]*ke[0]),5,16);
//	LCD_ShowNum(Temp_X+8*15, Temp_Y+16*2,(destination[1]*ke[1]),5,16);
//	
//	 LCD_ShowNum(Temp_X+8*3, Temp_Y+16*3,1,3,16);
//	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,steps,6,16);
//	 
//	 line_inter();//直线插补
//	 

//	 
// 	 	 for(i=0; i < NUM_AXIS; i++) {
// 		  current_position[i] =destination[i];  //保存当前坐标
// 	 }

// }
// 
// void circ_move(){
//	
//		u8 i=0;
//	 int k0[2]={1,1};
//	 int ke[2]={1,1};
//		f=0;
//	 GetO(current_position[0],current_position[1],destination[0],destination[1],inR);
//	 steps=GetSteps(mode);//圆弧步数

//	 //测试
// 	 //LCD_ShowString(2*10,2*10,200,12,12,"CODE G02!");
// 	 //LCD_ShowNum(2*10+50,2*10,steps,5,12);
//	 
// 
//	 
//	 
//	 //负数显示处理
//	for(i=0;i<2;i++){
//			if(current_position[i]<0) 
//			{	
//				k0[i]=-1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,'-',16,0);//显示负号
//			}else
//			{
//				k0[i]=1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16,' ',16,0);//显示负号
//			}
//			
//				if(destination[i]<0) 
//			{	
//				ke[i]=-1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,'-',16,0);//显示负号
//			}else
//			{
//				ke[i]=1;
//				LCD_ShowChar(Temp_X+8*3+(i)*8*11,Temp_Y+16*2,' ',16,0);//显示负号
//			}
//			
//	}
//	 //刷新参数x0 y0 xe ye
//	 LCD_ShowNum(Temp_X+8*4, Temp_Y+16,(current_position[0]*k0[0]),5,16);
//	LCD_ShowNum(Temp_X+8*15, Temp_Y+16,(current_position[1]*k0[1]),5,16);
//		LCD_ShowNum(Temp_X+8*4, Temp_Y+16*2,(destination[0]*ke[0]),5,16);
//	LCD_ShowNum(Temp_X+8*15, Temp_Y+16*2,(destination[1]*ke[1]),5,16);
//	
//	if(mode=='c')
//	 LCD_ShowNum(Temp_X+8*3, Temp_Y+16*3,2,3,16);
//	else 
//		 LCD_ShowNum(Temp_X+8*3, Temp_Y+16*3,3,3,16);
//	
//	LCD_ShowNum(Temp_X+8*14, Temp_Y+16*3,steps,6,16);
//	
//	 
//	 if(mode=='c')
//	 {
//		 circ_inter();//逆圆弧插补

//		}
//	 else if(mode=='n'){
//		 circr_inter();//顺圆弧插补

//	 }
//	 
//	 for(i=0; i < NUM_AXIS; i++) {
//  		  current_position[i] =destination[i];  //保存当前坐标
//			
//  	 }
//	 
//}



/////////////////
 
 #endif    //#ifndef __CNCCODE_H
