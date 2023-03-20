/*
 * @Author       : iPEK
 * @Date         : 2023-03-07
 * @LastEditTime : 2023-03-10
 * @Description  : Json编码、解码相关C文件，
 *				   CAN→TCP 		使用void Scheduler_Code(void)		此函数在can.c中的void CAN_RX_IRQHandler(void)接收中断调用
 * 				   TCP→CAN		使用void Scheduler_Decode(void)		此函数在usart3.c中的void USARTx_IRQHandler(void)接收中断调用
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */

#include <stdio.h>
#include <string.h>
#include "task.h"
#include "../common/cJSON.h"
#include "CAN_Spec/can_spec.h"

void RockerConversion(SCHAR *scV1, SCHAR *scV2, double angle, double power) // 遥感换算
{
	//	angle = (angle<0)?0:(angle>360)?360:angle;

	if ((angle > 0) && (angle < 90))
	{
		*scV1 = (SCHAR)(power * 100);
		*scV2 = (SCHAR)(power * (100 - 200 / 90.0 * angle));
	}
	else if ((angle < 180))
	{
		*scV1 = (SCHAR)(power * (300 - 200 / 90.0 * angle));
		*scV2 = (SCHAR)(-power * 100);
	}
	else if (angle < 270)
	{
		*scV1 = (SCHAR)(-power * 100);
		*scV2 = (SCHAR)(power * (200 / 90.0 * angle - 500));
	}
	else if (angle < 360)
	{
		*scV1 = (SCHAR)(power * (200 / 90.0 * angle - 700));
		*scV2 = (SCHAR)(power * 100);
	}
}

/**
 * @description  : 右侧操纵杆（控制小车）
 * @param         {cJSON*} STR_Payload:"payload":{"value":{"angle":57,"power":1},"what":"roverJoystick"}}
 *				  angle: 操纵杆转动角度（顺时针角度）
 *  			  what: 自定义组件名称
 * 				  power: 按压力度
 * @return        {*}
 */
static void roverJoystick_ENCODE(cJSON *STR_Payload) // 摇杆控制车
{
	cJSON *str_value = cJSON_GetObjectItem(STR_Payload, "value");
	double str_payload_angle = cJSON_GetObjectItem(str_value, "angle")->valuedouble;
	double str_payload_power = cJSON_GetObjectItem(str_value, "power")->valuedouble;
	SCHAR str_payload_scV1, str_payload_scV2;

	RockerConversion(&str_payload_scV1, &str_payload_scV2, str_payload_angle, str_payload_power);
#if DEBUG
	printf("angle:%.2f  power:%.2f scV1:%d scV2:%d\r\n", str_payload_angle, str_payload_power, str_payload_scV1, str_payload_scV2);
#endif
	SendCrawlerSpeedValue(str_payload_scV1, str_payload_scV2, 0);

#if DEBUG
	printf("MOVE_OBJECT_REQ_ENCODE\r\n");
#endif
}

/**
 * @description  : 开机命令
 * @return        {*}
 */
void Startup_CMD(void)
{
}

/**
 * @description  : 关机命令
 * @return        {*}
 */
void Shutdown_CMD(void)
{
}

/**
 * @description  : 开机程序
 * @return        {*}
 */
void StartUp(void)
{
}

/**
 * @description  : 关机程序
 * @return        {*}
 */
void ShutDown(void)
{
}

static void CHANGE_OBJECT_VALUE_REQ_ENCODE(cJSON *STR_Payload) // 开关机
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	if (str_payload_value)
	{
		Startup_CMD();
	}
	else
	{
		Shutdown_CMD();
	}
#if DEBUG
	printf("CHANGE_OBJECT_VALUE_REQ_ENCODE\r\n");
#endif
}

/**
 * @description  : 激光控制
 * @param         {cJSON*} STR_Payload:"payload":{"value":"50%","what":"laserIntensity"}
 *				  what: 自定义组件名称
 *				  value: 激光强度值，强度值范围0%，50%，75%，100%。
 * @return        {*}
 */
static void laserIntensity_ENCODE(cJSON *STR_Payload) // 激光控制
{

	char *str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valuestring;
	if (strcmp(str_payload_value, "100%") == 0)
		SendLaserPointEvent(3);
	else if (strcmp(str_payload_value, "75%") == 0)
		SendLaserPointEvent(2);
	else if (strcmp(str_payload_value, "50%") == 0)
		SendLaserPointEvent(1);
	else
		SendLaserPointEvent(0);

#if DEBUG
	printf("laserIntensity_ENCODE\r\n");
#endif
}

/**
 * @description  : 左侧定位功能
 * @param         {cJSON*} STR_Payload:"payload":{"value":"640Hz","what":"localizerFrequency"}
 *				  what: 自定义组件名称
 *				  value: dropdown下拉选择值(off/512Hz/640Hz/33kHz)
 * @return        {*}
 */
static void localizerFrequency_ENCODE(cJSON *STR_Payload) // 激光控制
{

	char *str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valuestring;
	if (strcmp(str_payload_value, "33kHz") == 0)
		SetLocatorConfigEvent(0x01, 0x0C);
	else if (strcmp(str_payload_value, "640Hz") == 0)
		SetLocatorConfigEvent(0x01, 0x0B);
	else if (strcmp(str_payload_value, "512Hz") == 0)
		SetLocatorConfigEvent(0x01, 0x0A);
	else
		SetLocatorConfigEvent(0x00, 0x00);
#if DEBUG
	printf("localizerFrequency_ENCODE\r\n");
#endif
}

/**
 * @description  : 辅助光源控制（前后辅助光源）
 * @param         {cJSON*} STR_Payload:"payload":{"value":1,"what":"auxiliaryLights"}
 *				  what: 自定义组件名称
 *				  value: 辅助光源灯光强度值，强度值范围0% - 100%
 * @return        {*}
 */
static void auxiliaryLights_ENCODE(cJSON *STR_Payload) // 前后辅助光源
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	str_payload_value = (str_payload_value < 0) ? 0 : (str_payload_value > 100) ? 100
																				: str_payload_value;
	SendExtraLightEvent(str_payload_value);
#if DEBUG
	printf("auxiliaryLights_ENCODE\r\n");
#endif
}

#define UpdateValueCOMMAND_NUM (sizeof(Update_Value_tasks) / sizeof(JsonDecode_task_t))
/**
 * @description  : 结构体数组，同属Update中的二级子指令，根据what值做二级判断
 * @param        void(*decode_func)(cJSON* STR_Payload)：相应控制指令的函数指针
 *				 messageName[30]：指令名称
 *
 */
static JsonDecode_task_t Update_Value_tasks[] = // 从上往下代表优先级
	{
		{roverJoystick_ENCODE, "roverJoystick"},		   // 摇杆控制车
		{auxiliaryLights_ENCODE, "auxiliaryLights"},	   // 前后辅助光源
		{laserIntensity_ENCODE, "laserIntensity"},		   // 激光控制
		{localizerFrequency_ENCODE, "localizerFrequency"}, // 左侧定位功能
};

/**
 * @description  : UPDATE解析，二级判断
 * @param         {cJSON*} STR_Payload:利用Payload中的what具体判断指令
 * @return        {*}
 */
static void UPDATE_VALUE_ENCODE(cJSON *STR_Payload) // 光源控制
{
	char *str_Payload_what = cJSON_GetObjectItem(STR_Payload, "what")->valuestring;
	for (uint8_t index = 0; index < UpdateValueCOMMAND_NUM; index++)
	{
		if (strcmp(str_Payload_what, Update_Value_tasks[index].messageName) == 0) // 找到对应指令
		{
			// 执行线程函数，使用的是函数指针
			Update_Value_tasks[index].decode_func(STR_Payload);
			break;
		}
		if (index == UpdateValueCOMMAND_NUM - 1)
		{
#if DEBUG
			printf("UPDATE_VALUE指令错误\r\n");
#endif
		}
	}

#if DEBUG
	printf("UPDATE_VALUE_ENCODE\r\n");
#endif
}

/**
 * @description  : 结构体数组，一级指令，根据messageName值做一级判断
 * @param        void(*decode_func)(cJSON* STR_Payload)：相应控制指令的函数指针
 *				 messageName[30]：指令名称
 *
 */
JsonDecode_task_t JsonDecode_tasks[] = // 从上往下代表优先级
	{
		{CHANGE_OBJECT_VALUE_REQ_ENCODE, "CHANGE_OBJECT_VALUE_REQ"}, // 开关机
		{UPDATE_VALUE_ENCODE, "UPDATE_VALUE"}						 // 光源控制
};

#define COMMAND_NUM (sizeof(JsonDecode_tasks) / sizeof(JsonDecode_task_t))

//{\"header\":{\"messageId\":34,\"messageName\":\"MOVE_OBJECT_REQ\",\"messageType\":\"CONTROL\"},\"payload\":{\"angle\":2,\"object\":\"crawlerJoystick\",\"power\":1}}	运动指令
//{\"header\":{\"messageId\":24,\"messageName\":\"CHANGE_OBJECT_VALUE_REQ\",\"messageType\":\"CONTROL\"},\"payload\":{\"object\":\"powerSwitch\",\"value\":true}}		开机指令
//{\"header\":{\"messageId\":24,\"messageName\":\"CHANGE_OBJECT_VALUE_REQ\",\"messageType\":\"CONTROL\"},\"payload\":{\"object\":\"powerSwitch\",\"value\":false}}		关机指令
//{"header":{"messageId":4,"messageName":"UPDATE_VALUE","messageType":"IPEK_CHINA_GUI"},"payload":{"value":1,"what":"lights"}}  										辅助光源控制（前后辅助光源）
//{"header":{"messageId":6,"messageName":"UPDATE_VALUE","messageType":"IPEK_CHINA_GUI"},"payload":{"value":"50%","what":"laserIntensity"}}								激光控制
uint32_t rec_cnt = 0;
uint32_t rec_err_cnt = 0;
uint32_t rec_errbreak_cnt = 0;
float err_rate = 0.0f;
float errbreak_rate = 0.0f;
char RECBuffLast[500] = {0};

/**
 * @description  : TCP→CAN		使用void Scheduler_Decode(void)		此函数在usart3.c中的void USARTx_IRQHandler(void)接收中断调用
 *								添加新的解码需要根据添加结构体数组和相应的解码函数
 * @return        {*}
 */
void Scheduler_Decode(cJSON *str_json)
{
#if DEBUG
	printf("Decode Task Run \r\n");
#endif
	// 循环判断所有线程，是否应该执行
	cJSON *str_header, *str_payload;
	//    if(DEBUG==1)printf("COMMAND_NUM:%d \r\n",COMMAND_NUM);
	rec_cnt++;

	if (!str_json)
	{
		rec_err_cnt++;
#if DEBUG
		printf("JSON格式错误-----------------------> \r\n "); // 输出json格式错误信息
#endif
	}
	else
	{
#if DEBUG
		printf("JSON格式正确:\r\n");
#endif
		str_header = cJSON_GetObjectItem(str_json, "header"); // 获取messageId键值对应的值的信息
		if (!str_header)
			return;
		char *str_header_messageName = cJSON_GetObjectItem(str_header, "messageName")->valuestring;
		for (uint8_t index = 0; index < COMMAND_NUM; index++)
		{
			if (strcmp(str_header_messageName, JsonDecode_tasks[index].messageName) == 0) // 找到对应指令
			{
				str_payload = cJSON_GetObjectItem(str_json, "payload"); // 获取age键对应的值的信息
				if (!str_payload)
					return;
				// 执行线程函数，使用的是函数指针
				JsonDecode_tasks[index].decode_func(str_payload);
				break;
			}
			if (index == COMMAND_NUM - 1)
			{
#if DEBUG
				printf("TCP指令错误\r\n");
#endif
			}
		}
	}
	err_rate = rec_err_cnt / (float)rec_cnt;
	errbreak_rate = rec_errbreak_cnt / (float)rec_cnt;
}
/*---------------------------------------TCP-CAN ↑------------------------------------*/
/*----------z*/

/*---------------------------------------CAN-TCP ↓------------------------------------*/

int MESSAGEID = 0;

void CANToWIFIDecode(CanRxMsg *rxMessage, uint8_t *CANToWIFIRECBuff)
{
	uint8_t Uart3_Cnt = 0;

	WiFiToCanID.StdId.data[0] = CANToWIFIRECBuff[Uart3_Cnt++];
	WiFiToCanID.StdId.data[1] = CANToWIFIRECBuff[Uart3_Cnt++];
	WiFiToCanID.StdId.data[2] = CANToWIFIRECBuff[Uart3_Cnt++];
	WiFiToCanID.StdId.data[3] = CANToWIFIRECBuff[Uart3_Cnt++];

	WiFiToCanID.ExtId.data[0] = CANToWIFIRECBuff[Uart3_Cnt++];
	WiFiToCanID.ExtId.data[1] = CANToWIFIRECBuff[Uart3_Cnt++];
	WiFiToCanID.ExtId.data[2] = CANToWIFIRECBuff[Uart3_Cnt++];
	WiFiToCanID.ExtId.data[3] = CANToWIFIRECBuff[Uart3_Cnt++];

	rxMessage->StdId = WiFiToCanID.StdId.id;
	rxMessage->ExtId = WiFiToCanID.ExtId.id;

	rxMessage->IDE = CANToWIFIRECBuff[Uart3_Cnt++]; // 使用扩展标识符
	rxMessage->RTR = CANToWIFIRECBuff[Uart3_Cnt++]; // 消息类型为数据帧，一帧8位
	rxMessage->DLC = CANToWIFIRECBuff[Uart3_Cnt++]; // 发送两帧信息
	for (uint8_t i = 0; i < rxMessage->DLC; i++)
	{
		rxMessage->Data[i] = CANToWIFIRECBuff[Uart3_Cnt++]; // 第一帧信息
	}

	// CanToWiFiID.StdId.id = RxMessage.StdId;

	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.StdId.data[0];
	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.StdId.data[1];
	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.StdId.data[2];
	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.StdId.data[3];

	// CanToWiFiID.ExtId.id = RxMessage.ExtId;
	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.ExtId.data[0];
	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.ExtId.data[1];
	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.ExtId.data[2];
	// UDPSendBuff[CAN_Cnt++] = CanToWiFiID.ExtId.data[3];

	// UDPSendBuff[CAN_Cnt++] = RxMessage.IDE;
	// UDPSendBuff[CAN_Cnt++] = RxMessage.RTR;
	// UDPSendBuff[CAN_Cnt++] = RxMessage.DLC;

	// for(uint8_t i = 0; i < RxMessage.DLC; i++)
	// {
	// 	UDPSendBuff[CAN_Cnt++]=RxMessage.Data[i];
	// }
}

/**
 * @description  : CMSG_METERCNT1VALUE编码
 * @return        {*}
 */
void CMSG_METERCNT1VALUE_CODE(void)
{
	cJSON *cjson_can = NULL;
	cJSON *cjson_header = NULL;
	cJSON *cjson_payload = NULL;
	char *TCPSendBuff = NULL;
	DataToFloat datatofloat;
	int vaule = 0;
	float vaule_f = 0.0f;
	for (uint8_t i = 0; i < RxMessage.DLC; i++)
	{
		datatofloat.data[i] = RxMessage.Data[i];
	}
	vaule = (datatofloat.value * 100 + 0.5f);
	vaule_f = vaule / 100.0f;
	cjson_can = cJSON_CreateObject();

	cjson_header = cJSON_CreateObject();
	cJSON_AddNumberToObject(cjson_header, "messageId", MESSAGEID);
	cJSON_AddStringToObject(cjson_header, "messageName", "CHANGE_METER_COUNTER_VALUE_REQ");
	cJSON_AddStringToObject(cjson_header, "messageType", "CONTROL");
	cJSON_AddItemToObject(cjson_can, "header", cjson_header);

	cjson_payload = cJSON_CreateObject();
	cJSON_AddFalseToObject(cjson_payload, "isLateral");
	cJSON_AddStringToObject(cjson_payload, "unit", "m");

	cJSON_AddNumberToObject(cjson_payload, "value", vaule_f);
	cJSON_AddItemToObject(cjson_can, "payload", cjson_payload);

	TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
	TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
	char *enter = "\n\0";
	strncat(TCPSendBuff, enter, 2);
	CAN_Cnt = strlen(TCPSendBuff);
#if DEBUG
	printf("CANBuff_cnt:%d \r\n", CAN_Cnt);

	printf("TCPSendBuff:%s \r\n", TCPSendBuff);
#endif
	/*-----------------
	-------------------
	-------------------发送函数请放这（发送TCPSendBuff）
													-------------------
													-------------------
													-----------------*/
	sendToApp(TCPSendBuff);
	cJSON_Delete(cjson_can); // 释放内存
	cJSON_free(TCPSendBuff);
}

/**
 * @description  : CAN→TCP 		使用void Scheduler_Code(void)		此函数在can.c中的void CAN_RX_IRQHandler(void)接收中断调用，
 *								添加新的编码需要根据CANID添加case和相应的编码函数
 * @return        {*}
 */
void Scheduler_Code(uint8_t *CANToWiFiRecBuff)
{
#if DEBUG
	printf("Code Task Run \r\n");
#endif
	CANToWIFIDecode(&RxMessage, CANToWiFiRecBuff);
	uint32_t CANID; // 区分任务

	if (RxMessage.IDE == CAN_ID_STD)
		CANID = RxMessage.StdId;
	else
		CANID = RxMessage.ExtId;

	// 接收成功
	MESSAGEID++;
	switch (CANID)
	{
	case CMSG_METERCNT1VALUE:
	{
		CMSG_METERCNT1VALUE_CODE();

		break;
	}
	default:
	{

#if DEBUG
		printf("CAN指令错误\r\n");
#endif
		MESSAGEID--;
		break;
	}
	}

	//			USART3_DMA_TxConfig((u32*)UDPSendBuff,CAN_Cnt);
	//			if(DEBUG==1)printf("接收到CAN%d帧数据\n",cnt_can);
}
//{"header":{"messageId":6,"messageName":"UPDATE_VALUE","messageType":"IPEK_CHINA_GUI"},"payload":{"value":"50%","what":"laserIntensity"}}
// "{\"header\":{\"messageId\":102,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":{\"angle\":57,\"power\":1},\"what\":\"roverJoystick\"}}"	//右侧操纵杆（控制小车）
//"{\"header\":{\"messageId\":5,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":1,\"what\":\"auxiliaryLights\"}}"	//
// 55 01 00 00 00 00 00 00 00 00 04 58 39 08 40 //CAN数据