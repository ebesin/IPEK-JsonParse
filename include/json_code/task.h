/*
 * @Author       : iPEK
 * @Date         : 2023-03-07
 * @LastEditTime : 2023-03-08
 * @Description  : Json编码、解码相关H文件�?
 *				   CAN→TCP 		使用void Scheduler_Code(void)		此函数在can.c�?的void CAN_RX_IRQHandler(void)接收�?�?调用
 * 				   TCP→CAN			使用void Scheduler_Decode(void)		此函数在usart3.c�?的void USARTx_IRQHandler(void)接收�?�?调用
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __Task_H
#define __Task_H
#include "../common/cJSON.h"
#include "CAN_Spec/can_spec.h"

typedef union
{
	uint8_t data[4];
	float value;
} DataToFloat;

typedef union
{
	uint8_t data[2];
	uint16_t value;
} DataTouInt16_t;

typedef union
{
	uint8_t data[2];
	int16_t value;
} DataToInt16_t;

/**
 * @description  : Json编码、解码相关全局变量
 */
typedef struct
{
	void (*decode_func)(cJSON *STR_Payload);
	char messageName[50];
} JsonDecode_task_t;

extern char *TCPSendBuff;
extern uint32_t rec_cnt;
extern uint32_t rec_err_cnt;
extern float err_rate;
extern float CAN_cntmeter, total_diff, section_diff;
/**
 * @description  : Json编码、解码相关函�?
 */
extern void Scheduler_Decode(cJSON *str_json);		   // Pad To MCU 调用
extern void Scheduler_Code(uint8_t *CANToWiFiRecBuff); // MCU To Pad 调用
// extern void RockerConversion(SCHAR *scV1, SCHAR *scV2, double angle, double power);
extern void StartUp(void);
extern void ShutDown(void);
extern int getCurrentTimeSMs(void);
extern unsigned long GetTickCount(void);
#endif
