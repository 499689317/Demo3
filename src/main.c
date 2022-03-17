
#include <STC89xx.h>
#include <mcs51/lint.h>
#include "Delay.h"
#include "LCD1602.h"
#include "Timer0.h"
// #include "Timer1.h"
// #include "E2PROM.h"
// #include "UART.h"
#include "Infrared.h"

void main() {

    LCD1602Init();
    // Timer1Init();
    // Timer1Run(0x01);
    // Timer1Close();
    // UARTInit();

    // unsigned short waddr = 0x2000;
    // unsigned short raddr = 0x2000;
    // unsigned short raddr2 = 0x2200;
    // unsigned char rev = 0x00;
    // unsigned char rev2[10];

    // E2PROMErase(raddr);
    // E2PROMErase(raddr2);

    // 测试结论：
    // 1. E2PROM在读写时是可以不区分扇区对其进行操作的，当读写时只需要控制地址自增或自减即可，扇区应该没有在硬件层面隔离，是一种虚拟人为按地址划分的概念
    // 2. 擦除操作时只能一次清除指定的扇区，擦除操作是针对扇区的，可能是为了考虑性能原因，擦除的性能并不好
    // 3. 实际使用时，应考虑存储一些静态数据，一次写入后大量读取的数据

    // 测试e2prom擦除是否会跨扇区
    // E2PROMRead(raddr2, rev2, 10);
    // for (int i = 0; i < 10; i++)
    // {
    //     Delay();
    //     UARTSendByte(rev2[i]);
    // }
    
    // 测试e2prom跨扇区读写
    // for (int i = 0; i < 520; i++)
    // {
    //     if (i < 515)
    //     {
    //         E2PROMWriteByte(waddr, 'H');
    //     }
    //     else
    //     {
    //         E2PROMWriteByte(waddr, 'Y');
    //     }
    //     waddr++;
    // }

    // Delay();
    // for (int i = 0; i < 520; i++)
    // {
    //     Delay();
    //     rev = E2PROMReadByte(raddr);
    //     raddr++;
    //     UARTSendByte(rev);
    // }
    
    // 测试红外接收设备
    InfraredInit();
    
    while (1)
    {
        if (InfraredReady())
        {
            InfraredReset();
            unsigned char x = InfraredData();
            LCD1602PrintUnsignedNum(1,1, x);
        }
        
    }
}

void InfraredInter() __interrupt(2) {
    InfraredLoop();
}

// void UARTInter() __interrupt(4) {
//     UARTLoop();
// }