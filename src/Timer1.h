/**
 * 定时器T1
 */

#ifndef TIMER1_H
#define TIMER1_H

// 初始化定时器
extern void Timer1Init();
// 是否启动定时器
extern void Timer1Run(unsigned char sign);
// 开启中断
extern void Timer1Open();
// 关闭中断
extern void Timer1Close();
// 循环计数
extern void Timer1Loop();

#endif
