/**
 * UART串口通信驱动声明
 */

#ifndef UART_H
#define UART_H

// 初始化串口参数
extern void UARTInit();
// 发送单字节
extern void UARTSendByte(unsigned char data);
// 发送多字节
extern void UARTSend(const unsigned char* data, unsigned char len);
// 接收多字节
extern void UARTReceive(unsigned char* data);

// 定时器循环
extern void UARTLoop();

#endif