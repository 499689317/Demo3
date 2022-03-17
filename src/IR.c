#include <STC89xx.h>
#include "LCD1602.h"
#include "Timer0.h"
#include "IR.h"

// 声明控制寄存器，红外接收信号输入线接到P3.3外部中断1引脚上
SBIT(IR_OUT, _P3, 3);
// 外部中断1控制位，TCON寄存器的IE1，外部中断请求位，IE1为1向cpu发起中断请求，处理完中断由软件置0
SBIT(IR_IE1, _TCON, 3);
// 外部中断1触发方式位，IT1为0低电平触发，1为下降沿触发
SBIT(IR_IT1, _TCON, 2);
// 外部中断1中断允许位
SBIT(IR_EX1, _IE, 2);
// 总中断
SBIT(IR_EA, _IE, 7);
// 中断优先级IP寄存器，外部中断1PX1控制位
SBIT(IR_PX1, _IP, 2);

// 红外接收输出高低电平
#define IR_LOW 0x00
#define IR_HIGH 0x01

// 红外接收状态
#define IR_IDLE 0x00
#define IR_PENDING 0x01// 待定状态，处于高电平状态，等待下降沿电平，当有下降沿电平时开始计时跳到ready状态
#define IR_READY 0x02// 准备状态，确认当前是否为开始状态或重复发送状态
#define IR_DECODE 0x03// 解码状态，解析数据状态，将红外信号的数据读取出来

// 当前接收器所处状态
unsigned char IR_STATUS = IR_PENDING;
// 定义一个标志区分是否要进入下一次检测
unsigned char IR_SIGN = 0x00;
// 当前已持续时间，T0累计次数
unsigned short IR_COUNT = 0x0000;
// 红外数据
unsigned char IR_Data[4];
unsigned char IR_pData;
unsigned char IR_DataFlag;
unsigned char IR_RepeatFlag;
unsigned char IR_Address;
unsigned char IR_Command;

// 红外接收装置在接收到38kMhz载波时，输入引脚输出低电平，其它情况输出高电平
void IRInit() {
    // 初始化T0计数器
    Timer0Tick();
    Timer0Close();
    Timer0Stop();// 先将T0关闭
    // 接收器输出引脚初始化，置为高电平
    IR_OUT = 0x01;
    // 开启外部中断1下降沿触发中断
    IR_IT1 = 0x01;
    // 中断1设置高优先级
    IR_PX1 = 0x01;
    // 开启外部中断1
    IR_EX1 = 0x01;
    // 开启总中断
    IR_EA = 0x01;
}

// 红外码头标识：9ms载波（9ms低电平）+4.5ms高电平总共13.5毫秒，11.0592MHz下为13500us/1.09us=12385
bool IRHeader() {
    return IR_COUNT >= (6238 - 500) && IR_COUNT < (6238 + 500);
}
// repeat标识：9ms载波（9ms低电平）+2.5ms高电平总共11.5ms，11500us/1.09us=10550
bool IRRepeat() {
    return IR_COUNT >= (10550 - 500) && IR_COUNT < (10550 + 500);
}
// 高电平，560us载波（560us低电平）+1680us高电平，2240us
bool IRHigh() {
    return IR_COUNT >= (2240 - 500) && IR_COUNT < (2240 + 500);
}
// 低电平，560us载波（560us低电平）+560us高电平，1120us
bool IRLow() {
    return IR_COUNT >= (1120 - 500) && IR_COUNT < (1120 + 500);
}

// 红外接收使用外部中断来接收信号，在中断中处理高低电平持续时间
// 红外接收器只有接收到频率为38kMHz的光波时才会输出低电平
void IRLoop() {
    LCD1602PrintHexNum(1, 1, IR_STATUS);
    switch (IR_STATUS)
    {
    case IR_PENDING:
        Timer0SetCounter(0);
        Timer0Start();// 开启TO计数
        IR_STATUS = IR_READY;
        break;
    case IR_READY:
        // 检测从上一次下降沿pending状态到本次下降沿持续的时间间隔
        IR_COUNT = Timer0GetCounter();
        LCD1602PrintUnsignedNum(1, 8, IR_COUNT);
        // 先把计数器清0
        Timer0SetCounter(0);
        
        // 判断时间是否符合红外开始引导信号
        // if (IRHeader())
        // {
        //     IR_STATUS = IR_DECODE;
        // }
        // else if (IRRepeat())
        // {
        //     // 重复状态
        //     IR_SIGN = 0x01;
        //     // 关闭计数器，反回重新检测
        //     Timer0Stop();
        //     IR_STATUS = IR_PENDING;
        // }
        // else
        // {
        //     IR_STATUS = IR_READY;
        // }
        break;
    case IR_DECODE:
        P1 = ~P1;
        // 解析数据
        IR_COUNT = Timer0GetCounter();
        // 再次清0计数器
        Timer0SetCounter(0);
        LCD1602PrintHexNum(2, 1, P1);
        LCD1602PrintUnsignedNum(2, 8, IR_COUNT);

        //如果计时为1120us，则接收到了数据0（判定值在12MHz晶振下为1120，在11.0592MHz晶振下为1032）
		if(IR_COUNT>1032-500 && IR_COUNT<1032+500)
		{
			IR_Data[IR_pData/8]&=~(0x01<<(IR_pData%8));	//数据对应位清0
			IR_pData++;			//数据位置指针自增
		}
		//如果计时为2250us，则接收到了数据1（判定值在12MHz晶振下为2250，在11.0592MHz晶振下为2074）
		else if(IR_COUNT>2074-500 && IR_COUNT<2074+500)
		{
			IR_Data[IR_pData/8]|=(0x01<<(IR_pData%8));	//数据对应位置1
			IR_pData++;			//数据位置指针自增
		}
		else					//接收出错
		{
			IR_pData=0;			//数据位置指针清0
			IR_STATUS=2;			//置状态为1
		}
		if(IR_pData>=32)		//如果接收到了32位数据
		{
			IR_pData=0;			//数据位置指针清0
			if((IR_Data[0]==~IR_Data[1]) && (IR_Data[2]==~IR_Data[3]))	//数据验证
			{
				IR_Address=IR_Data[0];	//转存数据
				IR_Command=IR_Data[2];
				IR_DataFlag=1;	//置收到连发帧标志位为1
			}
			Timer0Stop();		//定时器停止
			IR_STATUS=1;			//置状态为0
		}
        break;
    default:
        Timer0Stop();
        IR_STATUS = IR_PENDING;
        LCD1602PrintString(2, 14, "ERR");
        break;
    }
    // 中断处理结束，把IE1位置0，解码标志位置0
    IR_IE1 = 0x00;
    // IR_SIGN = 0x00;
}