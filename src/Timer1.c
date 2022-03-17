#include <STC89xx.h>
#include "Timer1.h"

// TCON控制位，选择使用哪个定时器，TF1, TR1为T0定时器控制位
SBIT(TIMER1_TF, _TCON, 7);// TF硬件置1，表示计数溢出，需要软件置0
SBIT(TIMER1_TR, _TCON, 6);// 置1，表示开启定时器
// TMOD寄存器，选择定时器模式，不可位寻址
SFR(TIMER1_TMOD, _TMOD);
// T1计数器
SFR(TIMER1_TH, _TH1);
SFR(TIMER1_TL, _TL1);
// 开启总中断控制位
SBIT(TIMER1_EA, _IE, 7);
// T1中断控制位，置1为开启中断
SBIT(TIMER1_ET, _IE, 3);

// 计算定时器溢出率
// 晶振频率为11.0592MHz，1秒钟11059200个时钟周期，12时钟周期 = 1机器周期
// #define TIMER1_FOSC 11059200
// #define TIMER1_BPS 9600
// 使用定时器模式2，自动重装计数
#define TIMER1_AUTO 0xFD//256 - (TIMER1_FOSC / 32 / 12 / TIMER1_BPS)

void Timer1Init() {
    // 选择模式2计数
    TIMER1_TMOD = 0x20;
    // 初始化计数器，计数器溢出后自动将TH重装入TL中
    TIMER1_TL = TIMER1_AUTO;
    TIMER1_TH = TIMER1_AUTO;
}

// 是否开启定时器，标志sign为0x00关闭定时器，为0x01开启定时器
void Timer1Run(unsigned char sign) {
    // 开始定时计数, TF置0，TR置1
    TIMER1_TF = 0x00;
    TIMER1_TR = sign;
}

// 开启中断
void Timer1Open() {
    TIMER1_EA = 0x01;
    TIMER1_ET = 0x01;
}

// 关闭中断
void Timer1Close() {
    // 关闭T1中断
    TIMER1_EA = 0x00;
    TIMER1_ET = 0x00;
}

// 循环定时器计数
void Timer1Loop() {
    TIMER1_TL = TIMER1_AUTO;
    TIMER1_TH = TIMER1_AUTO;
    // 软件置0
    TIMER1_TF = 0x00;
}