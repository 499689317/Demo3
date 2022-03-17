/**
 * 红外控制驱动声明
 */

#ifndef INFRARED_H
#define INFRARED_H

#define INFRARED_LOW 0x00
#define INFRARED_HIGH 0x01

#define INFRARED_16MS 16000
#define INFRARED_MIN_9MS 8000
#define INFRARED_MAX_9MS 10000
#define INFRARED_MIN_4P5MS 4000
#define INFRARED_MAX_4P5MS 5000
#define INFRARED_MIN_560US 300
#define INFRARED_MAX_560US 700
#define INFRARED_MIN_1680US 1300
#define INFRARED_MAX_1680US 1800

// 初始化红外接收
extern void InfraredInit();
// 读取数据
extern bool InfraredReady();
extern void InfraredReset();
extern unsigned char InfraredData();
// 循环接收红外信号
extern void InfraredLoop();

#endif