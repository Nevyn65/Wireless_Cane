
#ifndef	__GPIO_H
#define	__GPIO_H

#include	"delay.h"

/**********************
引脚别名定义
***********************/
sbit rs=P2^4;        //1602的数据/指令选择控制线 
sbit rw=P2^3;        //1602的读写控制线 
sbit en=P2^1;        //1602的使能控制线 

extern void lcd_init(void);    
extern void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
extern void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);

#endif
