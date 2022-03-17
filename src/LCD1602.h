/**
 * LCD 1602显示屏
 */

#ifndef LCD1602_H
#define LCD1602_H

// 初始化屏幕
extern void LCD1602Init();
// 显示一个字符
extern void LCD1602PrintChar(unsigned char row, unsigned char col, unsigned char data);
// 显示一个字符串
extern void LCD1602PrintString(unsigned char row, unsigned char col, const unsigned char* data);
// 显示一个无符号数字
extern void LCD1602PrintUnsignedNum(unsigned char row, unsigned char col, unsigned int data);
// 显示一个有符号数字
extern void LCD1602PrintSignedNum(unsigned char row, unsigned char col, int data);
// 显示一个16进制数
extern void LCD1602PrintHexNum(unsigned char row, unsigned char col, unsigned int data);
// 显示一个二进制数
extern void LCD1602PrintBinNum(unsigned char row, unsigned char col, unsigned int data);

#endif
