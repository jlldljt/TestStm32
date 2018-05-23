#ifndef __DELAY_H
#define __DELAY_H 			   
#include <sysdef.h>	  
//////////////////////////////////////////////////////////////////////////////////  
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���(֧��ucosii��ucosiii)
//����delay_us,delay_ms
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/5/2
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.1�޸�˵��
//�����˶�UCOSIII��֧��
////////////////////////////////////////////////////////////////////////////////// 	 
void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);
#define CDV_TIME_NUM 20

#ifdef CDV_TIME_NUM
//u32 GetCdvTimeTick(void);
//void SetCdvTimeTick(u32 timeTick);
u32 CalcTimeMS(u32 endTick , u32 startTick);
void StartCdvTime(u8 no);
u32 GetCdvTimeTick(void);
u32 EndCdvTime(u8 no);
void DelayTick(u32 num);
//extern u32 Sys_time_1ms;	//ϵͳ��ʱ1ms����
//extern u32 Sys_time_50ms;	//ϵͳ��ʱ1s����
extern u8 ValveAdjust_Flag;
#endif
#endif




























