/**
 * 红外控制驱动声明
 */

#ifndef IR_H
#define IR_H

// 初始化红外接收
extern void IRInit();
// 循环接收红外信号
extern void IRLoop();
// 返回接收数据
extern unsigned char IRAddr();
extern unsigned char IRCmd();

#endif