
#ifndef	__GPIO_H
#define	__GPIO_H

#include	"delay.h"

/**********************
���ű�������
***********************/
sbit rs=P2^4;        //1602������/ָ��ѡ������� 
sbit rw=P2^3;        //1602�Ķ�д������ 
sbit en=P2^1;        //1602��ʹ�ܿ����� 

extern void lcd_init(void);    
extern void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
extern void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);

#endif
