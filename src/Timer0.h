/**
 * 定时器T0
 */

#ifndef TIMER0_H
#define TIMER0_H

// 初始化定时器
extern void Timer0Init();
extern void Timer0Start();
extern void Timer0Stop();
extern void Timer0Open();
extern void Timer0Close();
// T0计数器
extern void Timer0Tick();
extern void Timer0SetCounter(unsigned short count);
extern unsigned short Timer0GetCounter();
// 循环计数
extern void Timer0Loop();

#endif
