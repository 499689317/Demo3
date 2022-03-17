#include <STC89xx.h>
#include "LCD1602.h"
#include "UART.h"

// 串口SCON特殊寄存器声明，可位寻址
SFR(UART_SCON, _SCON);
// 确定串口工作方式: SM0, SM1
// 00: 方式0，同步移位串行方式
// 01: 方式1，8位串行移位方式，波特率可变，2SMOD/32*(T1溢出率)
// 10: 方式2：9位UART
// 11: 方式3：9位UART，波特率可变，2SMOD/2SMOD/32*(T1溢出率)
SBIT(UART_SM0, _SCON, 7);
SBIT(UART_SM1, _SCON, 6);
// 控制串口工作位,由软件置位，0为禁止接收数据，1为允许接收数据
SBIT(UART_REN, _SCON, 4);
// 发送完成，请求中断标志位,由硬件置1，中断结束后必需要软件置0
SBIT(UART_TI, _SCON, 1);
// 接收完成，请求中断标志位，由硬件置1，中断结束后必需要软件置0
SBIT(UART_RI, _SCON, 0);
// 波特率选择特殊功能寄存器，不可位寻址
// SMOD：1波特率加倍，0波特率不加位
SFR(UART_PCON, _PCON);
// 串口中断控制位，1为允许串口中断，0为屏蔽串口中断
SBIT(UART_ES, _IE, 4);
// 总中断
SBIT(UART_EA, _IE, 7);
// 数据缓存寄存器
SFR(UART_SBUF, _SBUF);

// 数据缓冲区
unsigned char UART_REV_BUFF[8] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
unsigned char cursor1 = 0, cursor2 = 0;
unsigned int index1 = 0, index2 = 0;
// 准备状态标记位
unsigned char UART_READY = 0x00;

// 串口初始化
void UARTInit() {
    // 设置SCON，开启串口工作
    UART_REN = 0x01;
    // 设置串口工作模式1
	UART_SM0 = 0x00;
	UART_SM1 = 0x01;
	// 设置PCON,SMOD为1波特率加倍，0不加倍
	UART_PCON = 0x00;
    // 开启总中断
    UART_EA = 0x01;
    // 开启串口中断
    UART_ES = 0x01;
    // 缓存寄存器清0？？这种操作会发送出去一个数据
    // UART_SBUF = 0x00;
}

void UARTReady() {
    while (UART_READY == 0x01)
    {
        // 检测当前标志位是否是准备状态，0为可发送状诚，1为当前还有数据在发送中
    }
}

// 发送串口数据
void UARTSendByte(unsigned char data) {
    // 检测标志位
    UARTReady();
    // 发送数据时，将标志位置1
    UART_READY = 0x01;
    UART_SBUF = data;
    // 打印当前发送的总字节数
    index1++;
    LCD1602PrintUnsignedNum(1, 1, index1);
}
// 发送多字节，单次不超过255个字节
void UARTSend(const unsigned char* data, unsigned char len) {
    while (len--)
    {
        // 1个字节1个字节发送
        UARTSendByte(*data++);
    }
}

// 接收串口数据
void UARTReceiveByte() {
    // 打印当前接收缓存游标
    LCD1602PrintUnsignedNum(2, 1, cursor1);
    UART_REV_BUFF[cursor1] = UART_SBUF;
    // 打印当前接收字节
    LCD1602PrintHexNum(1, 13, UART_REV_BUFF[cursor1]);

    // 下一位缓存区
    cursor1++;
    if (cursor1 >= 8)
    {
        cursor1 = 0;
    }
    // 打印当前接收的总字节数
    index2++;
    LCD1602PrintUnsignedNum(1, 7, index2);
}

// 接收多字节，超过8个字节，重头开始接收
void UARTReceive(unsigned char* data) {
    while (UART_REV_BUFF[cursor2])
    {
        // 打印当前读取缓存游标
        LCD1602PrintUnsignedNum(2, 7, cursor2);
        *data++ = UART_REV_BUFF[cursor2];
        // 找印当前读取的缓存区
        LCD1602PrintHexNum(2, 13, UART_REV_BUFF[cursor2]);
        // 完成读取缓存后，清除缓存
        UART_REV_BUFF[cursor2] = 0x30;
        // 继续下一位缓存
        cursor2++;
        if (cursor2 >= 8)
        {
            cursor2 = 0;
        }
    }
}

void UARTLoop() {
    // 检测中断状态，软件将中断位置0
    // 完成接收数据，RI硬件置1
    if (UART_RI == 0x01)
    {
        // 退出中断
        UART_RI = 0x00;
        // 按字节接收数据
        UARTReceiveByte();
    }
    // 成完发送数据，TI硬件置1
    if (UART_TI == 0x01)
    {
        // 退出中断
        UART_TI = 0x00;
        // 发送完成置标志位0
        UART_READY = 0x00;
    }
}