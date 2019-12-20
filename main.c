#include	"delay.h"
#include	"lcd1602.h"

/**********************
引脚别名定义
***********************/
sbit ECHO=P4^6;       
sbit TRIG=P4^7;

sbit beep=P3^7;

#define Distance_MAX    700              //HC-SR04模块可检测的最大距离

uint8 code Cls[] =      {"                "};
uint8 code ASCII[15] =  {'0','1','2','3','4','5','6','7','8','9','.','-','M'};
uint8 code table[] =  {"the distance is:"};
			  
uint16  time=0;                          //测距用时间
uint32  S=0;                             //测距用距离
bit     flag =0;                         //测距用超出范围标识符
uint8   disbuff[4]	   ={ 0,0,0,0,};     //测距定义数组

/**************************************
功能描述：计算距离并在LCD1602屏上显示
入口参数：无
返回值：无
***************************************/

void fengming(uint16 x)		//蜂鸣器
{
	uint16 i,j;
	for(i=0;i<200;i++)
	{
		beep=~beep;
	   for(j=0;j<x;j++);
	}
	beep=1;
}

void Conut(void)
{
	 time=TH0*256+TL0;                  //计算出所用总时间
	 TH0=0;                             //清零定时器0装载值
	 TL0=0;                             //清零定时器0装载值
	
	 S=(time*1.7)/100;                  //算出来实测距离（单位是CM）
     if(S<100)                          //距离小于1m时，蜂鸣器响
     {
        fengming(500);		//蜂鸣器
     }
	 if((S>=Distance_MAX)||flag==1)     //实测距离超出测量范围显示“-”
	 {	 
	  flag=0;                           //清零测距用超出范围标识符
	 
	  DisplayOneChar(0, 1, ASCII[11]);
	  DisplayOneChar(1, 1, ASCII[10]);	//显示点
	  DisplayOneChar(2, 1, ASCII[11]);
	  DisplayOneChar(3, 1, ASCII[11]);
	  DisplayOneChar(4, 1, ASCII[12]);	//显示M
	 }
	 else
	 {
	  disbuff[0]=S%1000/100;      
	  disbuff[1]=S%1000%100/10;
	  disbuff[2]=S%1000%10 %10;
	  DisplayOneChar(0, 1, ASCII[disbuff[0]]);
	  DisplayOneChar(1, 1, ASCII[10]);	//显示点
	  DisplayOneChar(2, 1, ASCII[disbuff[1]]);
	  DisplayOneChar(3, 1, ASCII[disbuff[2]]);
	  DisplayOneChar(4, 1, ASCII[12]);	//显示M
	 }
}

/**************************************
功能描述：计数器0初始化函数
入口参数：无
返回值：无
***************************************/
void Timer0Init(void)
{
	AUXR &= 0x7F;                   //定时器0为12T模式
	TMOD &= 0xF8;                   //定时器0设置为定时方式
	TMOD &= 0xF7;                   //定时器0门控位GATE设置为0
	TMOD |= 0x01;		                //设定定时器0为16位不可重装载模式
	TL0 = 0;                        //12T模式下初始装载值       
	TH0 = 0;	                      //12T模式下初始装载值  
	ET0 = 1;                        //使能定时器0的溢出中断允许位
}

/**************************************
功能描述：计数器0中断服务函数
入口参数：无
返回值：无
***************************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
  flag=1;							   //测距用超出范围标识符置1
}
	
/**************************************
功能描述：启动HC-SR04模块 （控制控制口发一个10uS以上的高电平）
入口参数：无
返回值：无
***************************************/
void  StartModule(void) 		         
{
	TRIG=1;			                       //控制模块控制口Trig输出高电平
	delay_ms(5);                       //此延时不可少，需大于10uS
	TRIG=0;                            //控制模块控制口Trig输出低电平
}

/***************************************************************************
 * 描  述 : 主函数
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
int main(void)
{
/////////////////////////////////////////////////
//注意: STC15W4K32S4系列的芯片,上电后所有与PWM相关的IO口均为
//      高阻态,需将这些口设置为准双向口或强推挽模式方可正常使用
//相关IO: P0.6/P0.7/P1.6/P1.7/P2.1/P2.2
//        P2.3/P2.7/P3.7/P4.2/P4.4/P4.5
/////////////////////////////////////////////////
	P0M1 = 0;	P0M0 = 0;	              //设置P0.0~P0.7为准双向口
	P2M1  &= 0xE5;	P2M0 &= 0xE5;	    //设置P2.1,P2.3，P2.4为准双向口  
	P4M1 &= 0x3F;	P4M0 &= 0x3F;	      //设置P4.6~P4.7为准双向口

	delay_ms(100);	                   //上电延时		
	
  lcd_init();                        //液晶初始化 
  DisplayListChar(0, 0, table);      //在第一行靠最前显示字符串the distance is:

	delay_ms(100);                     //延时
	DisplayListChar(0, 1, Cls);	       //把第二行清空
	
  Timer0Init();                     //定时器0初始化
	EA=1;			                        //开启总中断			
	
	while(1)
	{
	     StartModule();               //启动HC-SR04模块 （控制控制口发一个10uS以上的高电平）
	     while(!ECHO);		            //当HC-SR04模块的接收端ECHO为低电平时，程序执行空语句，即等待ECHO端口变为高电平才执行下面的语句
	     TR0=1;			                  //当HC-SR04模块的接收端ECHO为高电平时，开启定时器0计数
	     while(ECHO);			            //若HC-SR04模块的接收端ECHO仍为高电平，程序执行空语句，即等待ECHO端口变为低电平才执行下面的语句
	     TR0=0;				                //当HC-SR04模块的接收端ECHO变为低电平时，关闭定时器0
       Conut();			                //计算距离并在LCD1602屏上显示
		   delay_ms(80);		            //80ms延时 
	}
}



                