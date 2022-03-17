/**
 * EEPROM驱动声明
 */

#ifndef E2PROM_H
#define E2PROM_H

// ISP/IAP特殊功能寄存器声明
// STC89xx.h中已经声明过这类寄存器了
// // 数据寄存器
// #define _ISP_DATA 0xE2
// SFR(ISP_DATA, 0xE2);
// // 地址寄存器
// #define _ISP_ADDRH 0xE3
// SFR(ISP_ADDRH, 0xE3);
// #define _ISP_ADDRL 0xE4
// SFR(ISP_ADDRL, 0xE4);
// // 命令寄存器
// #define _ISP_CMD 0xE5
// SFR(ISP_CMD, 0xE5);
// // 触发寄存器
// #define _ISP_TRIG 0xE6
// SFR(ISP_TRIG, 0xE6);
// // 控制寄存器
// #define _ISP_CONTR 0xE7
// SFR(ISP_CONTR, 0xE7);

// 读数据
extern unsigned char E2PROMReadByte(unsigned short addr);
extern void E2PROMRead(unsigned short addr, unsigned char* data, unsigned short len);
// 写数据
extern void E2PROMWriteByte(unsigned short addr, unsigned char data);
extern void E2PROMWrite(unsigned short addr, const unsigned char* data, unsigned short len);
// 擦除数据
extern void E2PROMErase(unsigned short addr);

#endif