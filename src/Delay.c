#include "Delay.h"
// #include "LCD1602.h"
// 单片机晶振频率为11.0592MHz，机器周期为12个时钟周期，12/11059200=1.09us
// 延时5ms大概为5000us/1.09=4587

// 从实验结果来看
// char类型在while中计数时，第一次循环占10次计数（10*1.09us）,从第2次循环开始每循环一次占6次计数（6*1.09us）
// short类型在while中计数时，第一次循环占15次计数（15*1.09us）,从第2次循环开始往后每循环一次占9次计数（9*1.09us）
// 所以5ms的循环次数约为5000us/9*1.09=510次循环
void Delay() {
    unsigned short n = 510;
    while (n > 0)
    {
        n--;
    }
}

void DelayXM(unsigned char xm) {
    // 这个公式计算出的结果误差很大，没法通用，需要直接计算出while循环次数
    unsigned short n = (xm * 1000) / 9.81;
    // LCD1602PrintUnsignedNum(1, 7, n);
    while (n > 0)
    {
        n--;
    }
}