#include <STC89xx.h>
#include "E2PROM.h"

// 总中断控制位
SBIT(E2PROM_EA, _IE, 7);
SFR(E2PROM_DATA, _ISP_DATA);
SFR(E2PROM_ADDRH, _ISP_ADDRH);
SFR(E2PROM_ADDRL, _ISP_ADDRL);
// 指令寄存器，MS1,MS0: 
// 00: 待机模式，不能进行读写操作
// 01: 应用程序对E2PROM读字节数据
// 10: 应用程序对E2PROM写字节数据
// 11: 应用程序对E2PROM擦除扇区数据
SFR(E2PROM_CMD, _ISP_CMD);
// 触发寄存器写入指令时需要连续写入，先关闭总中断
SFR(E2PROM_TRIG, _ISP_TRIG);
// 通过控制寄存器设置CPU等待时长及使能，该寄存器好像不能位寻址，试了下会出问题
SFR(E2PROM_CONTR, _ISP_CONTR);
// SBIT(E2PROM_ISPEN, _ISP_CONTR, 7);
// SBIT(E2PROM_SWBS, _ISP_CONTR, 6);
// SBIT(E2PROM_SWRST, _ISP_CONTR, 5);
// SBIT(E2PROM_WT2, _ISP_CONTR, 2);
// SBIT(E2PROM_WT1, _ISP_CONTR, 1);
// SBIT(E2PROM_WT0, _ISP_CONTR, 0);

// 停止
void E2PROMStop() {
    // 打开总中断，在触发IAP时，需要先把总中断关闭
    E2PROM_EA = 0x01;
    // 命令寄存器为待机模式，不能读写操作
    E2PROM_CMD = 0x00;
    // 触发寄存器清0
    E2PROM_TRIG = 0x00;
    // 使能位拉代
    // E2PROM_ISPEN = 0x00;
}

// 触发
void E2PROMTrigger() {
    // 关闭总中断，因为写入触发寄存器的两个指令必需要连续，不能被打断
    E2PROM_EA = 0x00;
    // 写入触发寄存器指令，开启ISP
    // 寄存器先写入0x46，再写入0xB9，ISP/IAP才会生效
    E2PROM_TRIG = 0x46;
    E2PROM_TRIG = 0xB9;
}

// 读数据
void E2PROMStartRead() {
    // data寄存器清0
    E2PROM_DATA = 0x00;
    // 当前操作指令为读操作
    E2PROM_CMD = 0x01;
    // 使能位拉高
    E2PROM_CONTR = 0x82;
    // E2PROM_ISPEN = 0x01;
    // E2PROM_SWBS = 0x00;
    // E2PROM_SWRST = 0x00;
    // 根据单片机晶振频率来设置读写等待CPU时长
    // 单片机晶振为11.0592MHz，< 20MHz
    // E2PROM_WT2 = 0x00;
    // E2PROM_WT1 = 0x01;
    // E2PROM_WT0 = 0x00;
}

// 读1字节
unsigned char E2PROMReadByte(unsigned short addr) {
    unsigned char data = 0x00;
    E2PROMStartRead();
    // 读取高位地址
    E2PROM_ADDRH = (unsigned char)(addr >> 8);
    // 读取低位地址
    E2PROM_ADDRL = (unsigned char)(addr & 0x00FF);
    // 触发读操作
    E2PROMTrigger();
    // 读字节
    data = E2PROM_DATA;
    // 停止ISP
    E2PROMStop();
    return data;
}

// 读多字节
void E2PROMRead(unsigned short addr, unsigned char* data, unsigned short len) {
    E2PROMStartRead();
    while (len--)
    {
        // 设置地址
        E2PROM_ADDRH = (unsigned char)(addr >> 8);
        E2PROM_ADDRL = (unsigned char)(addr & 0x00FF);
        // 触发读操作
        E2PROMTrigger();
        // 地址移动
        addr++;
        // 保存读取的数据
        *data++ = E2PROM_DATA;
    }
    // 结束操作
    E2PROMStop();
}

// 写数据
void E2PROMStartWrite() {
    // 当前操作指令为写操作
    E2PROM_CMD = 0x02;
    // 设置控制位，与读操作一样即可
    E2PROM_CONTR = 0x82;
    // 使能位置1
    // E2PROM_ISPEN = 0x01;
    // E2PROM_SWBS = 0x00;
    // E2PROM_SWRST = 0x00;
    // 设置等待时长
    // E2PROM_WT2 = 0x00;
    // E2PROM_WT1 = 0x01;
    // E2PROM_WT0 = 0x00;
}

// 写1字节
void E2PROMWriteByte(unsigned short addr, unsigned char data) {
    E2PROMStartWrite();
    // 设置地址
    E2PROM_ADDRH = (unsigned char)(addr >> 8);
    E2PROM_ADDRL = (unsigned char)(addr & 0x00FF);
    // 写入数据
    E2PROM_DATA = data;
    // 触发写操作
    E2PROMTrigger();
    // 停止ISP/IAP
    E2PROMStop();
}

// 写多字节
void E2PROMWrite(unsigned short addr, const unsigned char* data, unsigned short len) {
    E2PROMStartWrite();
    while (len--)
    {
        E2PROM_ADDRH = (unsigned char)(addr >> 8);
        E2PROM_ADDRL = (unsigned char)(addr & 0x00FF);
        // 写入字节
        E2PROM_DATA = *data++;
        // 地址移动
        addr++;
        // 触发写操作
        E2PROMTrigger();
    }
    // 结束操作
    E2PROMStop();
}

// 擦除数据
void E2PROMErase(unsigned short addr) {
    // 当前操作指令为拆除操作
    E2PROM_CMD = 0x03;
    // 设置控制位
    E2PROM_CONTR = 0x82;
    // 设置地址
    E2PROM_ADDRH = (unsigned char)(addr >> 8);
    E2PROM_ADDRL = (unsigned char)(addr & 0x00FF);
    // 触发操作
    E2PROMTrigger();
    // 停止ISP
    E2PROMStop();
}