# STM32-NC
STM32 NC


###1.数据协议
---
#####1.1 接收到数据头
 	>>01:文件开始传输
 	$$01:文件传输结束

###2.串口状态条尾部状态信号

	//状态条尾部蓝绿色闪烁：表示系统串口任务正在接收数据
	LCD_Fill(lcddev.width-19,lcddev.height-39,lcddev.width,lcddev.height-21,lcd_discolor[10-rec_count%2]); //填充状态信号

---
	//状态条尾部灰，橙色闪烁表示系统串口任务正在轮询工作
	LCD_Fill(lcddev.width-19,lcddev.height-39,lcddev.width,lcddev.height-21,lcd_discolor[13-com_num%2]); //填充状态信号
	
###3.超时后，文件接受终止
	本次接收的文件将丢弃 不会保存在本地
