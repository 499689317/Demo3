#include <STC89xx.h>
#include "LCD1602.h"

// RS：输入指令/数据选择位，0为指令寄存器，1为数据寄存器
SBIT(LCD1602_RS, _P3, 6);
// R/W：读写选择位，0为写操作，1为读操作
SBIT(LCD1602_RW, _P3, 5);
// EN：使能信号，高电位时读写数据/指令，负跳变是执行指令
SBIT(LCD1602_EN, _P3, 4);
// I/O：双向数据寄存器
SFR(LCD1602_IO, _P2);

// 向设备读取1字节数据，type：0x00为读取指令寄存器，0x01为读取数据寄存器
unsigned char LCD1602ReadByte(unsigned char type) {
    unsigned char data;
    // 当前读取指令寄存器还是数据寄存器
    LCD1602_RS = type;
    LCD1602_RW = 0x01;// 高电平为读取数据
    // 拉高使能信号
    LCD1602_EN = 0x01;
    // 读取P2寄存器数据
    data = LCD1602_IO;
    // 拉低使能
    LCD1602_EN = 0x00;
    // data: 0b0000 0000, 低7位为数据地址，第8位为使能状态，0为空闲中，1为非空闲状态
    return data;
}
// 检测当前是否为空闲状态
void LCD1602Ready() {
    LCD1602_IO = 0xFF;
    while (LCD1602ReadByte(0x00) & 0x80)
    {
        // 如果最高位为1，则一直循环阻塞，直到为0为止
    }
}

// 向设备写入1字节数据/指令，type: 0x00为写入指令，0x01为写入数据
void LCD1602WriteByte(unsigned char type, unsigned char data) {
    // 检测I/O口是否空闲
    LCD1602Ready();
    // 选择写数据还是写指令
    LCD1602_RS = type;
    // 选择当前为写模式
    LCD1602_RW = 0x00;
    // 将数据写入I/O口
    LCD1602_IO = data;
    // 拉高使能信号
    LCD1602_EN = 0x01;
    // 写入完成，51单片机一个机器周期大约1.09um，1602写入延时在nm级别，可以不需要延时函数
    LCD1602_EN = 0x00;
}

// 设置光标位置, row行：1-2，col列：1-16
// 1602屏幕为2行，16列，第一行地址范围为：0x00-0x27(可显示：0x00-0x0F)，第二行地址范围为：0x40-0x67(可显示：0x40-0x4F)
// 每写入一字节数据，游标地址要向前移动一位
void LCD1602Cursor(unsigned char row, unsigned char col) {
    if (row == 1)
    {
        // 第一行地址
        LCD1602WriteByte(0x00, (0x80 | (col - 1)));
    }
    else if (row == 2)
    {
        // 第二行地址
        LCD1602WriteByte(0x00, (0x80 | (0x40 + (col - 1))));
    }
}
// 实现一个x的y次方函数
int LCD1602Pow(int x, int y) {
    int n = 1;
    while (y > 0)
    {
        n *= x;
        y--;
    }
    return n;
}

// 初始化设备
void LCD1602Init() {
    // 2行显示，5X7点阵，8位I/O口
    LCD1602WriteByte(0x00, 0x38);
    // 显示屏幕，关闭光标
    LCD1602WriteByte(0x00, 0x0C);
    // 数据读写一个字节后，地址，光标自加1
    LCD1602WriteByte(0x00, 0x06);
    // 地址，光标，显示清0
    LCD1602WriteByte(0x00, 0x01);
}

// 显示一个字符
void LCD1602PrintChar(unsigned char row, unsigned char col, unsigned char data) {
    // 选择写入游标地址
    LCD1602Cursor(row, col);
    // 写入字符数据
    LCD1602WriteByte(0x01, data);
}

// 显示一个字符串
void LCD1602PrintString(unsigned char row, unsigned char col, const unsigned char* data) {
    LCD1602Cursor(row, col);
    while (*data)
    {
        LCD1602WriteByte(0x01, *data++);
    }
}

// 显示一个无符号数字
// data：4Byte，表示范围：0-65535，取data每一位作为一个字节显示到屏幕中，无符号需要占用5位
// c语言中0-9的整数int转成字符char，number + '0'，'0'的ASCII码为48
void LCD1602PrintUnsignedNum(unsigned char row, unsigned char col, unsigned int data) {
    LCD1602Cursor(row, col);
    unsigned char index = 5;
    unsigned int n = 0;
    while (index > 0)
    {
        index--;
        n = data / LCD1602Pow(10, index) % 10;
        LCD1602WriteByte(0x01, n + '0');
    }
}

// 显示一个有符号数字
// data: 4Byte, 表示范围：-32768~32767，包括符号需要占用6位
void LCD1602PrintSignedNum(unsigned char row, unsigned char col, int data) {
    LCD1602Cursor(row, col);
    unsigned char index = 5;
    int n = 0;
    // 判断data正负数
    if (data >= 0)
    {
        LCD1602WriteByte(0x01, '+');
    }
    else
    {
        LCD1602WriteByte(0x01, '-');
        // 为负数时转成正数处理
        data = -data;
    }
    while (index > 0)
    {
        index--;
        n = data / LCD1602Pow(10, index) % 10;
        LCD1602WriteByte(0x01, n + '0');
    }
}

// 显示一个16进制数
// data；4字节，表示范围：0x0000--0xFFFF，占用4位
void LCD1602PrintHexNum(unsigned char row, unsigned char col, unsigned int data) {
    LCD1602Cursor(row, col);
    unsigned char index = 4;
    unsigned int n = 0;
    // 16进制标记
    LCD1602WriteByte(0x01, '0');
    LCD1602WriteByte(0x01, 'x');
    while (index > 0)
    {
        index--;
        n = data / LCD1602Pow(16, index) % 16;
        if (n < 10)
        {
            LCD1602WriteByte(0x01, n + '0');
        }
        else
        {
            LCD1602WriteByte(0x01, n - 10 + 'A');
        }
    }
}

// 显示一个2进制数
// data: 4节节，表示范围：0b0000 0000 0000 0000-0b1111 1111 1111 1111，占用16位，因为显示屏只能显示16位，所以不标识0b前缀
void LCD1602PrintBinNum(unsigned char row, unsigned char col, unsigned int data) {
    LCD1602Cursor(row, col);
    unsigned char index = 16;
    unsigned int n = 0;
    while (index > 0)
    {
        index--;
        n = data / LCD1602Pow(2, index) % 2;
        LCD1602WriteByte(0x01, n + '0');
    }
}