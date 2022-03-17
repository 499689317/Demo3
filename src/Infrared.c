#include <STC89xx.h>
// #include "LCD1602.h"
#include "Timer0.h"
#include "Infrared.h"

// 声明控制寄存器，红外接收信号输入线接到P3.3外部中断1引脚上
SBIT(INFRARED_OUT, _P3, 3);
// 外部中断1控制位，TCON寄存器的IE1，外部中断请求位，IE1为1向cpu发起中断请求，处理完中断由软件置0
SBIT(INFRARED_IE1, _TCON, 3);
// 外部中断1触发方式位，IT1为0低电平触发，1为下降沿触发
SBIT(INFRARED_IT1, _TCON, 2);
// 外部中断1中断允许位
SBIT(INFRARED_EX1, _IE, 2);
// 总中断
SBIT(INFRARED_EA, _IE, 7);
// 中断优先级IP寄存器，外部中断1PX1控制位
SBIT(INFRARED_PX1, _IP, 2);

// 定义一个标志区分是否要进入下一次检测
unsigned char INFRARED_SIGN = 0x00;
// 红外数据
unsigned long INFRARED_DATA;

// 红外接收装置在接收到38kMhz载波时，输入引脚输出低电平，其它情况输出高电平
void InfraredInit() {
    // 初始化T0计数器
    Timer0Tick();
    Timer0Close();
    Timer0SetCounter(0);
    Timer0Stop();// 先将T0关闭
    // 接收器输出引脚初始化，置为高电平
    INFRARED_OUT = 0x01;
    // 开启外部中断1下降沿触发中断
    INFRARED_IT1 = 0x01;
    // 中断1设置高优先级
    INFRARED_PX1 = 0x01;
    INFRARED_IE1 = 0x00;
    // 开启外部中断1
    INFRARED_EX1 = 0x01;
    // 开启总中断
    INFRARED_EA = 0x01;
}

// 获取数据
bool InfraredReady() {
    return INFRARED_SIGN == 0x01;
}
void InfraredReset() {
    INFRARED_SIGN = 0x00;
}
unsigned char InfraredData() {
    return (unsigned char)(INFRARED_DATA >> 16);
}

// 红外计数器
unsigned short InfraredCounter(unsigned char sign) {
    Timer0SetCounter(0);
    Timer0Start();
    while (INFRARED_OUT == sign)
    {
        if (Timer0GetCounter() > INFRARED_16MS)
        {
            break;
        }
    }
    Timer0Stop();
    return Timer0GetCounter();
}

// 红外码头标识：9ms载波（9ms低电平）+4.5ms高电平总共13.5毫秒，11.0592MHz下为13500us/1.09us=12385
bool InfraredHeader(unsigned short TickLow, unsigned short TickHigh) {
    return (TickLow > INFRARED_MIN_9MS && TickLow <= INFRARED_MAX_9MS) && (TickHigh > INFRARED_MIN_4P5MS && TickHigh <= INFRARED_MAX_4P5MS);
}
// 红外线标识0
bool InfraredLow(unsigned short TickLow, unsigned short TickHigh) {
    return (TickLow > INFRARED_MIN_560US && TickLow <= INFRARED_MAX_560US) && (TickHigh > INFRARED_MIN_560US && TickHigh <= INFRARED_MAX_560US);
}
// 红外线标识1
bool InfraredHigh(unsigned short TickLow, unsigned short TickHigh) {
    return (TickLow > INFRARED_MIN_560US && TickLow <= INFRARED_MAX_560US) && (TickHigh > INFRARED_MIN_1680US && TickHigh <= INFRARED_MAX_1680US);
}

// 红外接收使用外部中断来接收信号，在中断中处理高低电平持续时间
// 红外接收器只有接收到频率为38kMHz的光波时才会输出低电平
void InfraredLoop() {
    unsigned short TickLow, TickHigh, i;
    TickLow = InfraredCounter(INFRARED_LOW);
    TickHigh = InfraredCounter(INFRARED_HIGH);
    if (!InfraredHeader(TickLow, TickHigh))
    {
        INFRARED_IE1 = 0x00;
        return;
    }
    // 解析数据
    for (i = 0; i < 32; i++)
    {
        TickLow = InfraredCounter(INFRARED_LOW);
        TickHigh = InfraredCounter(INFRARED_HIGH);
        // 直接用或比较要比与比较快
        if (TickLow < INFRARED_MIN_560US || TickLow > INFRARED_MAX_560US || TickHigh < INFRARED_MIN_560US || TickHigh > INFRARED_MAX_1680US)
        {
            INFRARED_IE1 = 0x00;
            return;
        }
        
        // if (!(InfraredLow(TickLow, TickHigh) || InfraredHigh(TickLow, TickHigh)))
        // {
        //     INFRARED_IE1 = 0x00;
        //     return;
        // }

        INFRARED_DATA >>= 1;
        if (TickHigh > INFRARED_MIN_1680US)
        {
            INFRARED_DATA |= 0x80000000;
        }
    }
    INFRARED_IE1 = 0x00;
    INFRARED_SIGN = 0x01;
}