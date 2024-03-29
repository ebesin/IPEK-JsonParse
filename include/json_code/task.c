/*
 * @Author       : iPEK
 * @Date         : 2023-03-07
 * @LastEditTime : 2023-07-04
 * @Description  : Json编码、解码相关C文件，
 *				   CAN→TCP 		使用void Scheduler_Code(void)		此函数在can.c中的void CAN_RX_IRQHandler(void)接收中断调用
 * 				   TCP→CAN		使用void Scheduler_Decode(void)		此函数在usart3.c中的void USARTx_IRQHandler(void)接收中断调用
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#include <time.h>
#include <sys/time.h>
#include <memory.h>

#include <stdio.h>
#include <string.h>
#include "task.h"
#include "../common/cJSON.h"
#include "CAN_Spec/can_spec.h"
float CAN_cntmeter = 0.0f;
float total_diff = 0.0f;
float section_diff = 0.0f;
uint8_t cruise_flag = 0;
uint8_t cntmeter_flag = 1; // 初始值为1，用于判断是否可以设置全局和区域计米器数值
unsigned long GetTickCount(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);

	// struct timeval tp;
	// gettimeofday(&tp, 0);

	// return tp.tv_usec;
}
/**
 * @description  : 小车遥感换算函数，0-360
 * @param         {SCHAR*} scV1:速度scV1
 * @param         {SCHAR*} scV2:速度scV2
 * @param         {double} angle:摇杆偏移角度
 * @param         {double} power:摇杆离开初始点的距离
 * @return        {*}
 */
void RockerConversion_360(SCHAR *scV1, SCHAR *scV2, double angle, double power) // 遥感换算
{

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
 * @description  : 相机遥感换算函数，0-360
 * @param         {SCHAR*} scV1:速度scV1
 * @param         {SCHAR*} scV2:速度scV2
 * @param         {double} angle:摇杆偏移角度
 * @param         {double} power:摇杆离开初始点的距离
 * @return        {*}
 */
void RockerConversion_180_Camera(SCHAR *scV1, SCHAR *scV2, double angle, double power) // 遥感换算
{

	angle -= 90;
	if (angle < 0)
		angle += 360;
	if ((angle >= 0) && (angle <= 90))
	{
		*scV1 = (SCHAR)(power * 100 / 90 * angle);
		*scV2 = (SCHAR)(power * (100 - 100 / 90 * angle));
	}
	else if ((angle < 180))
	{
		*scV1 = (SCHAR)(power * (200 - 100 / 90 * angle));
		*scV2 = (SCHAR)(power * (100 - 100 / 90 * angle));
	}
	else if (angle < 270)
	{
		*scV1 = (SCHAR)(power * (200 - 100 / 90 * angle));
		*scV2 = (SCHAR)(power * (100 / 90 * angle - 300));
	}
	else if (angle < 360)
	{
		*scV1 = (SCHAR)(power * (100 / 90 * angle - 400));
		*scV2 = (SCHAR)(power * (100 / 90 * angle - 300));
	}
}
/**
 * @description  : 相机遥感换算函数，-180-180
 * @param         {SCHAR*} scV1:速度scV1
 * @param         {SCHAR*} scV2:速度scV2
 * @param         {double} angle:摇杆偏移角度
 * @param         {double} power:摇杆离开初始点的距离
 * @return        {*}
 */
void RockerConversion_180_Camera_Test(SCHAR *scV1, SCHAR *scV2, double angle, double power) // 遥感换算
{
	if ((angle >= -45) && (angle < 45))
	{
		*scV1 = (SCHAR)(power);
		*scV2 = (SCHAR)(0);
	}
	else if ((angle < -45) && (angle >= -135))
	{
		*scV1 = (SCHAR)(0);
		*scV2 = (SCHAR)(power);
	}
	else if ((angle >= 135) || (angle < -135))
	{
		*scV1 = (SCHAR)(-power);
		*scV2 = (SCHAR)(0);
	}
	else if ((angle < 135) && (angle >= 45))
	{
		*scV1 = (SCHAR)(0);
		*scV2 = (SCHAR)(-power);
	}
}
/**
 * @description  : 小车遥感换算函数，-180-180
 * @param         {SCHAR*} scV1:速度scV1
 * @param         {SCHAR*} scV2:速度scV2
 * @param         {double} angle:摇杆偏移角度
 * @param         {double} power:摇杆离开初始点的距离
 * @return        {*}
 */
void RockerConversion_180_Car(SCHAR *scV1, SCHAR *scV2, double angle, double power) // 遥感换算
{
	angle -= 90;
	if (angle < 0)
		angle += 360;
	if ((angle >= 0) && (angle < 90))
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
 * @description  : 小车遥感换算函数，-180-180
 * @param         {SCHAR*} scV1:速度scV1
 * @param         {SCHAR*} scV2:速度scV2
 * @param         {double} angle:摇杆偏移角度
 * @param         {double} power:摇杆离开初始点的距离
 * @return        {*}
 */
void RockerConversion_180_Car_Test(SCHAR *scV1, SCHAR *scV2, double angle, double power) // 遥感换算
{
	if ((angle >= -45) && (angle < 45))
	{
		*scV1 = (SCHAR)(power);
		*scV2 = (SCHAR)(-power);
	}
	else if ((angle < -45) && (angle >= -135))
	{
		*scV1 = (SCHAR)(power);
		*scV2 = (SCHAR)(power);
	}
	else if ((angle >= 135) || (angle < -135))
	{
		*scV1 = (SCHAR)(-power);
		*scV2 = (SCHAR)(power);
	}
	else if ((angle < 135) && (angle >= 45))
	{
		*scV1 = (SCHAR)(-power);
		*scV2 = (SCHAR)(-power);
	}
}

/**
 * @description  : 右侧操纵杆（控制小车）
 * @param         {cJSON*} "payload":{"value":{"angle":-98.10724803565803,"power":1},"what":"roverJoystick"}
 *				  angle: 操纵杆转动角度（顺时针角度）
 *  			  what: 自定义组件名称
 * 				  power: 按压力度
 * @return        {*}
 */
static void roverJoystick_ENCODE(cJSON *STR_Payload) // 右侧操纵杆（控制小车）
{
	cJSON *str_value = cJSON_GetObjectItem(STR_Payload, "value");
	double str_payload_angle = cJSON_GetObjectItem(str_value, "angle")->valuedouble;
	double str_payload_power = cJSON_GetObjectItem(str_value, "power")->valuedouble * 0.6;
	SCHAR str_payload_scV1, str_payload_scV2;

	RockerConversion_180_Car_Test(&str_payload_scV1, &str_payload_scV2, str_payload_angle, str_payload_power);
#if DEBUG
	printf("angle:%.2f  power:%.2f scV1:%d scV2:%d\r\n", str_payload_angle, str_payload_power, str_payload_scV1, str_payload_scV2);
#endif
	// if((str_payload_scV1 != 0 )&&(str_payload_scV2 != 0))
	// cntmeter_flag = 0;
	// else
	// cntmeter_flag = 1;

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
	SendStartupCMD();
}

/**
 * @description  : 关机命令
 * @return        {*}
 */
void Shutdown_CMD(void)
{
	SendPowerOffCMD();
}
/**
 * @description  : 开关机指令
 * @param         {cJSON*} "payload":{"object":"powerSwitch","value":true}
 *				  object: 电源开关
 *  			  value: 开关状态
 * @return        {*}
 */
static void CHANGE_OBJECT_VALUE_REQ_ENCODE(cJSON *STR_Payload) // 开关机指令
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
static void localizerFrequency_ENCODE(cJSON *STR_Payload) // 左侧定位功能
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
 * @param         {cJSON*} STR_Payload:"payload":{"value":1,"what":"auxiliaryLightsValueInPercent"}
 *				  what: 自定义组件名称
 *				  value: 辅助光源灯光强度值，强度值范围0% - 100%
 * @return        {*}
 */
static void auxiliaryLightsValueInPercent_ENCODE(cJSON *STR_Payload) // 前后辅助光源
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	str_payload_value = (str_payload_value < 0) ? 0 : (str_payload_value > 100) ? 100
																				: str_payload_value;
	SendExtraLightEvent(str_payload_value);
#if DEBUG
	printf("auxiliaryLights_ENCODE\r\n");
#endif
}

/**
 * @description  : 主灯控制（前后主灯）
 * @param         {cJSON*} STR_Payload:"payload":{"value":26,"what":"highBeamMainLightsValueInPercent"}
 *				  what: 自定义组件名称
 *				  value: 主灯灯光强度值，强度值范围0% - 100%
 * @return        {*}
 */
static void highBeamMainLightsValueInPercent_ENCODE(cJSON *STR_Payload) // 主灯控制（前后主灯）
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	str_payload_value = (str_payload_value < 0) ? 0 : (str_payload_value > 100) ? 100
																				: str_payload_value;
	SendMainLightEvent(str_payload_value);
#if DEBUG
	printf("highBeamMainLightsValueInPercent_ENCODE\r\n");
#endif
}

/**
 * @description  : 自动灯光按钮关闭，调节近光灯调节
 * @param         {cJSON*} STR_Payload:"payload":{"value":77,"what":"lowBeamMainLightsValueInPercent"}
 *				  what: 自定义组件名称
 *				  value: 灯光强度调节范围0% - 100%
 * @return        {*}
 */
static void lowBeamMainLightsValueInPercent_ENCODE(cJSON *STR_Payload) // 自动灯光按钮关闭，调节近光灯调节
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	str_payload_value = (str_payload_value < 0) ? 0 : (str_payload_value > 100) ? 100
																				: str_payload_value;
	SendlowBeamMainLightsEvent(str_payload_value);
#if DEBUG
	printf("lowBeamMainLightsValueInPercent_ENCODE\r\n");
#endif
}
/**
 * @description  :   自动灯光切换按钮
 * @param         {cJSON*} STR_Payload:"payload":{"value":26,"what":"autoAngleMainLightsStatus"}
 *				  what: 自定义组件名称
 *				  value: true/false 自动灯光开启和关闭
 * @return        {*}
 */
static void autoAngleMainLightsStatus_ENCODE(cJSON *STR_Payload) //   自动灯光切换按钮
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	SendautoAngleMainLightsStatus(str_payload_value);
#if DEBUG
	printf("autoAngleMainLightsStatus_ENCODE\r\n");
#endif
}

/**
 * @description  : 近光灯控制
 * @param         {cJSON*} STR_Payload:"payload":{"value":30,"what":"autoAngleMainLightsValueInDegrees"}
 *				  what: 自定义组件名称
 *				  value: 角度调价值范围0 - 45°
 * @return        {*}
 */
static void autoAngleMainLightsValueInDegrees_ENCODE(cJSON *STR_Payload) // 近光灯控制
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	str_payload_value = (str_payload_value < 0) ? 0 : (str_payload_value > 45) ? 45
																			   : str_payload_value;
	SendautoAngleMainLightsValueInDegrees(str_payload_value);
#if DEBUG
	printf("autoAngleMainLightsValueInDegrees_ENCODE\r\n");
#endif
}

/**
 * @description  : 自动模式，线缆盘张力控制
 * @param         {cJSON*} STR_Payload:"payload":{"value":1,"what":"cableReelPower"}
 *				  what: 自定义组件名称
 *				  value: 提升张力指令（发送具体张力值，张力值范围0% - 100%，待UI更新）
 * @return        {*}
 */
static void cableReelPower_ENCODE(cJSON *STR_Payload) // 自动模式，线缆盘张力控制
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	str_payload_value = (str_payload_value < 0) ? 0 : (str_payload_value > 100) ? 100
																				: str_payload_value;
	SendReelPower(str_payload_value);
#if DEBUG
	printf("cableReelPower_ENCODE\r\n");
#endif
}

/**
 * @description  : 手动模式，线缆盘速度控制
 * @param         {cJSON*} STR_Payload:"payload":{"value":-13,"what":"cableReelSpeed"}
 *				  what: 自定义组件名称
 *				  value: 提升速度指令（发送具体速度值，速度值范围-100% - 100%，待UI更新）
 * @return        {*}
 */
static void cableReelSpeed_ENCODE(cJSON *STR_Payload) // 手动模式，线缆盘速度控制
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	str_payload_value = (str_payload_value < -100) ? -100 : (str_payload_value > 100) ? 100
																					  : str_payload_value;
	SendReelSpeed(str_payload_value);
#if DEBUG
	printf("cableReelSpeed_ENCODE\r\n");
#endif
}

/**
 * @description  : 手动模式自动模式切换按钮
 * @param         {cJSON*} STR_Payload:"payload":{"value":"manual","what":"cableReelType"}
 *				  what: 自定义组件名称
 *				  value: automatic为自动模式，manual为手动模式
 * @return        {*}
 */
static void cableReelType_ENCODE(cJSON *STR_Payload) // 手动模式自动模式切换按钮
{

	char *str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valuestring;
	if (strcmp(str_payload_value, "automatic") == 0) //
		SendReelFunctionCodeEvent(0);
	else
	{
		SendReelFunctionCodeEvent(1);
		SendFullStop();
		// cntmeter_flag = 1;
	}
#if DEBUG
	printf("cableReelType_ENCODE\r\n");
#endif
}

/**
 * @description  : 左侧操纵杆（控制摄像头）
 * @param         {cJSON*} STR_Payload:"payload":{"value":{"angle":-60.945395900922854,"power":0.46330335634441494},"what":"cameraJoystick"}
 *				  angle: 操纵杆转动角度（顺时针角度）-180~180°，浮点型
 *  			  what: 自定义组件名称
 * 				  power: 按压力度,0.0001~1，Float型
 * @return        {*}
 */
static void cameraJoystick_ENCODE(cJSON *STR_Payload) // 左侧操纵杆（控制摄像头）
{
	cJSON *str_value = cJSON_GetObjectItem(STR_Payload, "value");
	double str_payload_angle = cJSON_GetObjectItem(str_value, "angle")->valuedouble;
	double str_payload_power = cJSON_GetObjectItem(str_value, "power")->valuedouble;
	SCHAR str_payload_scV1, str_payload_scV2;

	RockerConversion_180_Camera_Test(&str_payload_scV1, &str_payload_scV2, str_payload_angle, str_payload_power);
#if DEBUG
	printf("angle:%.2f  power:%.2f scV1:%d scV2:%d\r\n", str_payload_angle, str_payload_power, str_payload_scV1, str_payload_scV2);
#endif
	SendCameraJoystickEvent(str_payload_scV1, str_payload_scV2);

#if DEBUG
	printf("MOVE_OBJECT_REQ_ENCODE\r\n");
#endif
}

/**
 * @description  : 开启/关闭定速巡航
 * @param         {cJSON*} STR_Payload:"payload":{"value":true,"what":"cruiseControlStatus"}
 *				  what: 自定义组件名称
 *				  value: true为开启定速巡航，false为关闭定速巡航（操作小车Joystick会自动发送取消定速巡航指令）
 * @return        {*}
 */
static void cruiseControlStatus_ENCODE(cJSON *STR_Payload) // 开启/关闭定速巡航
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	if (str_payload_value) //
	{
		SendCrawlerSpeedValue(0, 0, 1);
		cruise_flag = 1;
	}

	else
	{
		SendCrawlerSpeedValue(0, 0, 0);
		cruise_flag = 0;
	}

#if DEBUG
	printf("cruiseControlStatus_ENCODE\r\n");
#endif
}

/**
 * @description  : 设置定速巡航速度值
 * @param         {cJSON*} STR_Payload:"payload":{"value":36,"what":"cruiseControlValue"}
 *				  what: 自定义组件名称
 *				  value: 设置定速巡航速度值
 * @return        {*}
 */
static void cruiseControlValue_ENCODE(cJSON *STR_Payload) // 设置定速巡航速度值
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	// if(str_payload_value == 0)
	// cntmeter_flag = 1;
	// else
	// cntmeter_flag = 0;
	SendCrawlerSpeedValue(str_payload_value, str_payload_value, 1);
#if DEBUG
	printf("cruiseControlValue_ENCODE\r\n");
#endif
}

/**
 * @description  : 手动模式自动模式切换按钮
 * @param         {cJSON*} STR_Payload:"payload":{"value":"manual","what":"focusType"}
 *				  what: 自定义组件名称
 *				  value: auto代表自动模式，manual代表手动模式
 * @return        {*}
 */
static void focusType_ENCODE(cJSON *STR_Payload) // 手动模式自动模式切换按钮
{

	char *str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valuestring;
	if (strcmp(str_payload_value, "manual") == 0) //
		SetManualFocus();
	else
		SetAutoFocus();
#if DEBUG
	printf("focusType_ENCODE\r\n");
#endif
}

/**
 * @description  : 右侧离合器开关
 * @param         {cJSON*} STR_Payload:"payload":{"value":true,"what":"clutchStatus"}
 *				  what: 自定义组件名称
 *				  value: 离合器开关(true or false)
 * @return        {*}
 */
static void clutchStatus_ENCODE(cJSON *STR_Payload) // 右侧离合器开关
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	if (str_payload_value) //
		SendClutchOpen(0xff);
	else
		SendClutch(0);
#if DEBUG
	printf("clutchStatus_ENCODE\r\n");
#endif
}

/**
 * @description  : 前置摄像头切换为后置摄像头
 * @param         {cJSON*} STR_Payload:"payload":{"value":"rear","what":"cameraChosen"}
 *				  what: 自定义组件名称
 *				  value: 后置摄像头名称（切换后置摄像头默认开启第一个后置摄像头）
 * @return        {*}
 */
static void cameraChosen_ENCODE(cJSON *STR_Payload) // 前置摄像头切换为后置摄像头
{

	char *str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valuestring;
	if (strcmp(str_payload_value, "front") == 0) //
		SendBackViewCameraSwitchEvent(0);
	else
		SendBackViewCameraSwitchEvent(1);
#if DEBUG
	printf("cameraChosen_ENCODE\r\n");
#endif
}

/**
 * @description  : 切换第二个后置摄像头
 * @param         {cJSON*} STR_Payload:"payload":{"value":2,"what":"rearCameraIdx"}
 *				  what: 自定义组件名称
 *				  value: 第二个后置摄像头
 * @return        {*}
 */
static void rearCameraIdx_ENCODE(cJSON *STR_Payload) // 切换第二个后置摄像头
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	SendBackViewCameraSwitchEvent(str_payload_value);
#if DEBUG
	printf("clutchEnabled_ENCODE\r\n");
#endif
}

/**
 * @description  : 自动灯光关闭，调节远光灯数值（自动灯光切换按钮为ON时）
 * @param         {cJSON*} STR_Payload:"payload":{"value":56,"what":"autoHighBeamMainLightsValueInPercent"}
 *				  what: 自定义组件名称
 *				  value: 主灯灯光强度值，强度值范围0 - 100
 * @return        {*}
 */
static void autoHighBeamMainLightsValueInPercent_ENCODE(cJSON *STR_Payload) // 自动灯光关闭，调节远光灯数值（自动灯光切换按钮为ON时）
{

	int str_payload_value = cJSON_GetObjectItem(STR_Payload, "value")->valueint;
	SendAutoHighBeamMainLightsValueInPercentEvent(str_payload_value);
#if DEBUG
	printf("clutchEnabled_ENCODE\r\n");
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
		{cameraJoystick_ENCODE, "cameraJoystick"},											   // 左侧操纵杆（控制摄像头）
		{roverJoystick_ENCODE, "roverJoystick"},											   // 摇杆控制车
		{auxiliaryLightsValueInPercent_ENCODE, "auxiliaryLightsValueInPercent"},			   // 前后辅助光源
		{laserIntensity_ENCODE, "laserIntensity"},											   // 激光控制
		{localizerFrequency_ENCODE, "localizerFrequency"},									   // 左侧定位功能
		{highBeamMainLightsValueInPercent_ENCODE, "highBeamMainLightsValueInPercent"},		   // 主灯控制（前后主灯）
		{lowBeamMainLightsValueInPercent_ENCODE, "lowBeamMainLightsValueInPercent"},		   // 近光灯调节
		{autoAngleMainLightsStatus_ENCODE, "autoAngleMainLightsStatus"},					   // 自动灯光切换按钮
		{autoAngleMainLightsValueInDegrees_ENCODE, "autoAngleMainLightsValueInDegrees"},	   // 近光灯控制
		{cableReelPower_ENCODE, "cableReelPower"},											   // 自动模式，线缆盘张力控制
		{cableReelSpeed_ENCODE, "cableReelSpeed"},											   // 手动模式，线缆盘速度控制
		{cableReelType_ENCODE, "cableReelType"},											   // 手动模式自动模式切换按钮
		{cruiseControlStatus_ENCODE, "cruiseControlStatus"},								   // 开启/关闭定速巡航
		{cruiseControlValue_ENCODE, "cruiseControlValue"},									   // 设置定速巡航速度值
		{focusType_ENCODE, "focusType"},													   // 切换手动模式和自动模式
		{clutchStatus_ENCODE, "clutchStatus"},												   // 右侧离合器开关
		{cameraChosen_ENCODE, "cameraChosen"},												   // 前置摄像头切换为后置摄像头
		{rearCameraIdx_ENCODE, "rearCameraIdx"},											   // 切换第二个后置摄像头
		{autoHighBeamMainLightsValueInPercent_ENCODE, "autoHighBeamMainLightsValueInPercent"}, // 切换第二个后置摄像头
};

/**
 * @description  : UPDATE解析，二级判断
 * @param         {cJSON*} STR_Payload:利用Payload中的what具体判断指令
 * @return        {*}
 */
static void UPDATE_VALUE_ENCODE(cJSON *STR_Payload) // 光源控制
{
	char *str_Payload_what = cJSON_GetObjectItem(STR_Payload, "what")->valuestring;
	if (!str_Payload_what)
		return;
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
 * @description  : 'reset' 摄像头恢复正常
 * @param         {cJSON*} STR_Payload:"payload":{"action":"reset","what":"camera"}
 *				  action: 'reset' 摄像头恢复正常
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void camera_reset_ENCODE(cJSON *STR_Payload) // 'reset' 摄像头恢复正常
{
	SendCameraHomePositionEvent();
#if DEBUG
	printf("camera_reset_ENCODE\r\n");
#endif
}

/**
 * @description  : 结构体数组，同属action中的三级子指令，根据action值做三级级判断
 * @param        void(*decode_func)(cJSON* STR_Payload)：相应控制指令的函数指针
 *				 messageName[30]：指令名称
 *
 */
static JsonDecode_task_t camera_tasks[] = // 从上往下代表优先级
	{
		{camera_reset_ENCODE, "reset"}, // 摄像头恢复正常

};
#define cameraCOMMAND_NUM (sizeof(camera_tasks) / sizeof(JsonDecode_task_t))
/**
 * @description  : action解析，三级判断
 * @param         {cJSON*} STR_Payload:利用Payload中的action具体判断指令
 * @return        {*}
 */
static void camera_ENCODE(cJSON *STR_Payload) // action
{
	char *str_Payload_action = cJSON_GetObjectItem(STR_Payload, "action")->valuestring;
	if (!str_Payload_action)
		return;

	for (uint8_t index = 0; index < cameraCOMMAND_NUM; index++)
	{
		if (strcmp(str_Payload_action, camera_tasks[index].messageName) == 0) // 找到对应指令
		{
			// 执行线程函数，使用的是函数指针
			camera_tasks[index].decode_func(STR_Payload);
			break;
		}
		if (index == cameraCOMMAND_NUM - 1)
		{
#if DEBUG
			printf("camera指令错误\r\n");
#endif
		}
	}

#if DEBUG
	printf("camera_ENCODE\r\n");
#endif
}

/**
 * @description  : 按下increment按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"incrementing_started","what":"zoom"}
 *				  action:  incrementing_started 组件设置的值（数值可以调整，待确定）
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void zoom_incrementing_started_ENCODE(cJSON *STR_Payload) //  按下increment按钮发送指令
{
	SendCameraZoomEvent(-100);
#if DEBUG
	printf("zoom_incrementing_started_ENCODE\r\n");
#endif
}

/**
 * @description  : 松开increment按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"incrementing_ended","what":"zoom"}
 *				  action:  incrementing_ended 组件设置的值
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void zoom_incrementing_ended_ENCODE(cJSON *STR_Payload) //  松开increment按钮发送指令
{
	SendCameraZoomEvent(0);
#if DEBUG
	printf("zoom_incrementing_ended_ENCODE\r\n");
#endif
}

/**
 * @description  : 按下decrementing按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"decrementing_started","what":"zoom"}
 *				  action:  decrementing_started 组件设置的值（数值可以调整，待确定）
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void zoom_decrementing_started_ENCODE(cJSON *STR_Payload) //  按下decrementing按钮发送指令
{
	SendCameraZoomEvent(100);
#if DEBUG
	printf("zoom_decrementing_started_ENCODE\r\n");
#endif
}

/**
 * @description  : 松开decrementing按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"decrementing_ended","what":"zoom"}
 *				  action:  incrementing_started 组件设置的值（数值可以调整，待确定）
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void zoom_decrementing_ended_ENCODE(cJSON *STR_Payload) //  松开decrementing按钮发送指令
{
	SendCameraZoomEvent(0);
#if DEBUG
	printf("zoom_decrementing_ended_ENCODE\r\n");
#endif
}

/**
 * @description  : 结构体数组，同属zoom中的三级子指令，根据action值做三级判断
 * @param        void(*decode_func)(cJSON* STR_Payload)：相应控制指令的函数指针
 *				 messageName[30]：指令名称
 *
 */
static JsonDecode_task_t zoom_tasks[] = // 从上往下代表优先级
	{
		{zoom_incrementing_started_ENCODE, "incrementing_started"}, // 摄像头恢复正常
		{zoom_incrementing_ended_ENCODE, "incrementing_ended"},
		{zoom_decrementing_started_ENCODE, "decrementing_started"},
		{zoom_decrementing_ended_ENCODE, "decrementing_ended"},
};
#define zoomCOMMAND_NUM (sizeof(zoom_tasks) / sizeof(JsonDecode_task_t))
/**
 * @description  : action解析，三级判断
 * @param         {cJSON*} STR_Payload:利用Payload中的action具体判断指令
 * @return        {*}
 */
static void zoom_ENCODE(cJSON *STR_Payload) // action
{
	char *str_Payload_action = cJSON_GetObjectItem(STR_Payload, "action")->valuestring;
	if (!str_Payload_action)
		return;

	for (uint8_t index = 0; index < zoomCOMMAND_NUM; index++)
	{
		if (strcmp(str_Payload_action, zoom_tasks[index].messageName) == 0) // 找到对应指令
		{
			// 执行线程函数，使用的是函数指针
			zoom_tasks[index].decode_func(STR_Payload);
			break;
		}
		if (index == zoomCOMMAND_NUM - 1)
		{
#if DEBUG
			printf("zoom指令错误\r\n");
#endif
		}
	}

#if DEBUG
	printf("zoom_ENCODE\r\n");
#endif
}

/**
 * @description  : 按下increment按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"incrementing_started","what":"focus"}
 *				  action:  固定参数incrementing_started
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void focus_incrementing_started_ENCODE(cJSON *STR_Payload) //  按下increment按钮发送指令
{
	SetManualFocus();
	SendFastFocusEvent(100);
#if DEBUG
	printf("focus_incrementing_started_ENCODE\r\n");
#endif
}

/**
 * @description  : 松手increment按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"incrementing_ended","what":"focus"}
 *				  action:  固定参数incrementing_ended
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void focus_incrementing_ended_ENCODE(cJSON *STR_Payload) //  松手increment按钮发送指令
{
	SendFastFocusEvent(0);
#if DEBUG
	printf("focus_incrementing_ended_ENCODE\r\n");
#endif
}

/**
 * @description  : 按下decrement按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"decrementing_started","what":"focus"}
 *				  action:  固定参数decrementing_started
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void focus_decrementing_started_ENCODE(cJSON *STR_Payload) //  按下decrement按钮发送指令
{
	SetManualFocus();
	SendFastFocusEvent(-100);
#if DEBUG
	printf("focus_decrementing_started_ENCODE\r\n");
#endif
}

/**
 * @description  : 松开decrement按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"decrementing_started","what":"focus"}
 *				  action:  固定参数decrementing_started
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void focus_decrementing_ended_ENCODE(cJSON *STR_Payload) //  松开decrement按钮发送指令
{
	SendFastFocusEvent(0);
#if DEBUG
	printf("focus_decrementing_ended_ENCODE\r\n");
#endif
}

/**
 * @description  : 结构体数组，同属focus中的三级子指令，根据action值做三级判断
 * @param        void(*decode_func)(cJSON* STR_Payload)：相应控制指令的函数指针
 *				 messageName[30]：指令名称
 *
 */
static JsonDecode_task_t focus_tasks[] = // 从上往下代表优先级
	{
		{focus_incrementing_started_ENCODE, "incrementing_started"}, // 摄像头恢复正常
		{focus_incrementing_ended_ENCODE, "incrementing_ended"},
		{focus_decrementing_started_ENCODE, "decrementing_started"},
		{focus_decrementing_ended_ENCODE, "decrementing_ended"},
};
#define focusCOMMAND_NUM (sizeof(focus_tasks) / sizeof(JsonDecode_task_t))
/**
 * @description  : action解析，三级判断
 * @param         {cJSON*} STR_Payload:利用Payload中的action具体判断指令
 * @return        {*}
 */
static void focus_ENCODE(cJSON *STR_Payload) // action
{
	char *str_Payload_action = cJSON_GetObjectItem(STR_Payload, "action")->valuestring;
	if (!str_Payload_action)
		return;

	for (uint8_t index = 0; index < focusCOMMAND_NUM; index++)
	{
		if (strcmp(str_Payload_action, focus_tasks[index].messageName) == 0) // 找到对应指令
		{
			// 执行线程函数，使用的是函数指针
			focus_tasks[index].decode_func(STR_Payload);
			break;
		}
		if (index == focusCOMMAND_NUM - 1)
		{
#if DEBUG
			printf("focus指令错误\r\n");
#endif
		}
	}

#if DEBUG
	printf("focus_ENCODE\r\n");
#endif
}

/**
 * @description  : 升降架上升指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"incrementing_started","what":"elevator"}
 *				  action:  升降架上升incrementing_started
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void elevator_incrementing_started_ENCODE(cJSON *STR_Payload) //  升降架上升指令
{
	SendLiftSpeedValue(100);
#if DEBUG
	printf("elevator_incrementing_started_ENCODE\r\n");
#endif
}

/**
 * @description  : 松开上升按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"incrementing_ended","what":"elevator"}
 *				  action:  升降架上升incrementing_ended
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void elevator_incrementing_ended_ENCODE(cJSON *STR_Payload) //  松开上升按钮发送指令
{
	SendLiftSpeedValue(0);
#if DEBUG
	printf("elevator_incrementing_ended_ENCODE\r\n");
#endif
}

/**
 * @description  :   升降架下降指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"decrementing_started","what":"elevator"}
 *				  action:  升降架下降decrementing_started
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void elevator_decrementing_started_ENCODE(cJSON *STR_Payload) //  升降架下降指令
{
	SendLiftSpeedValue(-100);
#if DEBUG
	printf("elevator_decrementing_started_ENCODE\r\n");
#endif
}

/**
 * @description  : 松开下降按钮发送指令
 * @param         {cJSON*} STR_Payload:"payload":{"action":"decrementing_ended","what":"elevator"}
 *				  action:  incrementing_started 组件设置的值（数值可以调整，待确定）
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void elevator_decrementing_ended_ENCODE(cJSON *STR_Payload) //  松开下降按钮发送指令
{
	SendLiftSpeedValue(0);
#if DEBUG
	printf("elevator_decrementing_ended_ENCODE\r\n");
#endif
}

/**
 * @description  : 结构体数组，同属elevator中的三级子指令，根据action值做三级判断
 * @param        void(*decode_func)(cJSON* STR_Payload)：相应控制指令的函数指针
 *				 messageName[30]：指令名称
 *
 */
static JsonDecode_task_t elevator_tasks[] = // 从上往下代表优先级
	{
		{elevator_incrementing_started_ENCODE, "incrementing_started"}, // 摄像头恢复正常
		{elevator_incrementing_ended_ENCODE, "incrementing_ended"},
		{elevator_decrementing_started_ENCODE, "decrementing_started"},
		{elevator_decrementing_ended_ENCODE, "decrementing_ended"},
};
#define elevatorCOMMAND_NUM (sizeof(elevator_tasks) / sizeof(JsonDecode_task_t))
/**
 * @description  : action解析，三级判断
 * @param         {cJSON*} STR_Payload:利用Payload中的action具体判断指令
 * @return        {*}
 */
static void elevator_ENCODE(cJSON *STR_Payload) // action
{
	char *str_Payload_action = cJSON_GetObjectItem(STR_Payload, "action")->valuestring;
	if (!str_Payload_action)
		return;

	for (uint8_t index = 0; index < elevatorCOMMAND_NUM; index++)
	{
		if (strcmp(str_Payload_action, elevator_tasks[index].messageName) == 0) // 找到对应指令
		{
			// 执行线程函数，使用的是函数指针
			elevator_tasks[index].decode_func(STR_Payload);
			break;
		}
		if (index == elevatorCOMMAND_NUM - 1)
		{
#if DEBUG
			printf("elevator指令错误\r\n");
#endif
		}
	}

#if DEBUG
	printf("elevator_ENCODE\r\n");
#endif
}

/**
 * @description  : 结构体数组，同属what中的二级子指令，根据what值做二级判断
 * @param        void(*decode_func)(cJSON* STR_Payload)：相应控制指令的函数指针
 *				 messageName[30]：指令名称
 *
 */
static JsonDecode_task_t Action_tasks[] = // 从上往下代表优先级
	{
		{camera_ENCODE, "camera"},	   // 摄像头恢复正常
		{zoom_ENCODE, "zoom"},		   // 左侧camera zoom
		{focus_ENCODE, "focus"},	   // 左侧camera focus
		{elevator_ENCODE, "elevator"}, // 右侧升降架高度调节
};
#define ActionCOMMAND_NUM (sizeof(Action_tasks) / sizeof(JsonDecode_task_t))

/**
 * @description  : ACTION解析，二级判断
 * @param         {cJSON*} STR_Payload:利用Payload中的action具体判断指令
 * @return        {*}
 */
static void ACTION_ENCODE(cJSON *STR_Payload) // action
{
	char *str_Payload_action = cJSON_GetObjectItem(STR_Payload, "what")->valuestring;
	if (!str_Payload_action)
		return;

	for (uint8_t index = 0; index < ActionCOMMAND_NUM; index++)
	{
		if (strcmp(str_Payload_action, Action_tasks[index].messageName) == 0) // 找到对应指令
		{
			// 执行线程函数，使用的是函数指针
			Action_tasks[index].decode_func(STR_Payload);
			break;
		}
		if (index == ActionCOMMAND_NUM - 1)
		{
#if DEBUG
			printf("ACTION指令错误\r\n");
#endif
		}
	}

#if DEBUG
	printf("ACTION_ENCODE\r\n");
#endif
}

/**
 * @description  : 'reset' 摄像头恢复正常
 * @param         {cJSON*} STR_Payload:"payload":{"action":"reset","what":"camera"}
 *				  action: 'reset' 摄像头恢复正常
 *				  what: 自定义组件名称
 * @return        {*}
 */
static void EMERGENCY_STOP_ENCODE(cJSON *STR_Payload) // 'reset' 摄像头恢复正常
{
	SendFullStop();
	// cntmeter_flag = 1;
#if DEBUG
	printf("EMERGENCY_STOP_ENCODE\r\n");
#endif
}

/**
 * @description  : START_VIDEO_STREAMING_RESP_ENCODE
 * @param         {cJSON*} STR_Payload:"payload":{"ip":"192.168.16.100"}
 * @return        {*}
 */
static void START_VIDEO_STREAMING_RESP_ENCODE(cJSON *STR_Payload) // 开机回复
{
	onVideoStreamConfirm();

#if DEBUG
	printf("START_VIDEO_STREAMING_RESP_ENCODE\r\n");
#endif
}

/**
 * @description  : APPLICATION_CLOSED_ENCODE
 * @param         {cJSON*} STR_Payload:"payload":{"ip":"192.168.16.100"}
 * @return        {*}
 */
static void APPLICATION_CLOSED_ENCODE(cJSON *STR_Payload) // 关机
{
	Shutdown_CMD();
	CAN_cntmeter = 0.0f;
	total_diff = 0.0f;
	section_diff = 0.0f;
	// cntmeter_flag = 1;
#if DEBUG
	printf("APPLICATION_CLOSED_ENCODE\r\n");
#endif
}

/**
 * @description  : CHANGE_METER_COUNTER_VALUE_REQ
 * @param         {cJSON*} STR_Payload:"payload":{"unit":"m","value":7}
 * @return        {*}
 */
unsigned int SetTime = 0;
unsigned int RecTime = 0;
static void CHANGE_METER_COUNTER_VALUE_REQ_ENCODE(cJSON *STR_Payload) // 改变参考数值 section_countmeter
{
	double section_countmeter_value = cJSON_GetObjectItem(STR_Payload, "value")->valuedouble;
	SetTime = GetTickCount();
	cntmeter_flag = (SetTime - RecTime) > 500 ? 1 : 0;
	if (cntmeter_flag)
	{
		section_diff = section_countmeter_value - CAN_cntmeter;
#if DEBUGSEND
		SEND_DEBUG_INFO("设置成功,section_diff:%f\r\n", section_diff);
#endif
#if DEBUG
		printf("设置成功,section_diff:%f\r\n", section_diff);
#endif
	}
	else
	{
#if DEBUGSEND
		SEND_DEBUG_INFO("设置失败,section_diff:%f\r\n", section_diff);
#endif
#if DEBUG
		printf("设置失败,section_diff:%f\r\n", section_diff);
#endif
	}

#if DEBUG
	printf("CHANGE_METER_COUNTER_VALUE_REQ_ENCODE\r\n");
#endif
}

/**
 * @description  : CHANGE_METER_COUNTER_VALUE_REQ
 * @param         {cJSON*} STR_Payload:"payload":{"unit":"m","value":7}
 * @return        {*}
 */
static void CHANGE_TOTAL_METER_COUNTER_VALUE_REQ_ENCODE(cJSON *STR_Payload) // 改变参考数值 section_countmeter
{
	double section_countmeter_value = cJSON_GetObjectItem(STR_Payload, "value")->valuedouble;
	SetTime = GetTickCount();
	cntmeter_flag = (SetTime - RecTime) > 500 ? 1 : 0;
	if (cntmeter_flag)
	{
		total_diff = section_countmeter_value - CAN_cntmeter;
#if DEBUGSEND
		SEND_DEBUG_INFO("设置成功,total_diff:%f\r\n", total_diff);
#endif
#if DEBUG
		printf("设置成功,total_diff:%f\r\n", total_diff);
#endif
	}
	else
	{
#if DEBUGSEND
		SEND_DEBUG_INFO("设置失败,total_diff:%f\r\n", total_diff);
#endif
#if DEBUG
		printf("设置失败,total_diff:%f\r\n", total_diff);
#endif
	}
#if DEBUG
	printf("CHANGE_TOTAL_METER_COUNTER_VALUE_REQ_ENCODE\r\n");
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

		{CHANGE_OBJECT_VALUE_REQ_ENCODE, "CHANGE_OBJECT_VALUE_REQ"},						   // 开关机
		{EMERGENCY_STOP_ENCODE, "EMERGENCY_STOP"},											   // Full stop
		{UPDATE_VALUE_ENCODE, "UPDATE_VALUE"},												   // UPDATE_VALUE
		{ACTION_ENCODE, "ACTION"},															   // ACTION
		{START_VIDEO_STREAMING_RESP_ENCODE, "START_VIDEO_STREAMING_RESP"},					   // 开机回复
		{APPLICATION_CLOSED_ENCODE, "APPLICATION_CLOSED"},									   // 关机处理
		{CHANGE_METER_COUNTER_VALUE_REQ_ENCODE, "CHANGE_METER_COUNTER_VALUE_REQ"},			   // 改变参考数值 section_countmeter
		{CHANGE_TOTAL_METER_COUNTER_VALUE_REQ_ENCODE, "CHANGE_TOTAL_METER_COUNTER_VALUE_REQ"}, // 改变参考数值 section_countmeter
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
}

/**
 * @description  : CMSG_LIFTPOSITION_ELEVATOR编码，升降架高度和百分比显示
 * @return        {*}
 */
void CMSG_LIFTPOSITION_ELEVATOR_CODE(void)
{
	cJSON *cjson_can = NULL;
	cJSON *cjson_header = NULL;
	cJSON *cjson_payload = NULL;
	char *TCPSendBuff = NULL;
	// DataTouInt16_t datatoint16;
	DataToInt16_t datatoint16;
	char scvalue;
	for (uint8_t i = 0; i < 2; i++)
	{
		datatoint16.data[i] = RxMessage.Data[i];
	}
	scvalue = RxMessage.Data[5];
	scvalue = scvalue >= 0 && scvalue <= 100 && datatoint16.value >= 0 ? scvalue : 0;

	#if DEBUGSEND
	if(scvalue==0)
	{
		SEND_DEBUG_INFO("**************************ERROR*********************\r\n");
		SEND_DEBUG_INFO("CANID:%x \r\n", RxMessage.ExtId);
		SEND_DEBUG_INFO("elevatorValueInPercent:%d ,elevatorValueInMm:%d \r\n",RxMessage.Data[5],datatoint16.value);
		SEND_DEBUG_INFO("***************************************************\r\n");
		// printf("**************************ERROR*********************\r\n");	
		// printf("CANID:%x \r\n", RxMessage.ExtId);
		// printf("elevatorValueInPercent:%d ,elevatorValueInMm:%d \r\n",RxMessage.Data[5],datatoint16.value);
		// printf("***************************************************\r\n");
//		printf("CANID:%x \r\n", RxMessage.ExtId);
//		printf("Data:");
	}

	#endif
	cjson_can = cJSON_CreateObject();

	cjson_header = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson_header, "messageType", "IPEK_CHINA_GUI");
	cJSON_AddStringToObject(cjson_header, "messageName", "UPDATE_VALUE");

	cJSON_AddItemToObject(cjson_can, "header", cjson_header);

	cjson_payload = cJSON_CreateObject();

	cJSON_AddStringToObject(cjson_payload, "what", "elevatorValueInPercent");
	cJSON_AddNumberToObject(cjson_payload, "value", scvalue);

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

	cJSON_ReplaceItemInObject(cjson_payload, "what", cJSON_CreateString("elevatorValueInMm"));
	cJSON_ReplaceItemInObject(cjson_payload, "value", cJSON_CreateNumber(datatoint16.value >= 0 ? datatoint16.value : 0));

	TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
	TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
	strncat(TCPSendBuff, enter, 2);
	CAN_Cnt = strlen(TCPSendBuff);
	sendToApp(TCPSendBuff);
#if DEBUG
	printf("CANBuff_cnt:%d \r\n", CAN_Cnt);

	printf("TCPSendBuff:%s \r\n", TCPSendBuff);
#endif

	cJSON_Delete(cjson_can); // 释放内存
	cJSON_free(TCPSendBuff);
}

// float FloatToString(float Nub)
// {
// 	char NubStrBuff[32];
// 	char* NubStrBuff1;
// 	float NubFloat;
// 	sprintf(NubStrBuff, "%.2f", Nub);
// 	NubFloat = strtof(NubStrBuff,&NubStrBuff1);
// 	// cJSON_free(NubStrBuff1);
// 	return NubFloat;
// }

/**
 * @description  : CMSG_METERCNT1VALUE编码，左侧米计数器功能（待确定）
 * @return        {*}
 */
void CMSG_METERCNT1VALUE_CODE(void)
{
	cJSON *cjson_can = NULL;
	cJSON *cjson_header = NULL;
	cJSON *cjson_payload = NULL;
	char *TCPSendBuff = NULL;
	DataToFloat datatofloat;
	float vaule_f = 0.0f;
	for (uint8_t i = 0; i < RxMessage.DLC; i++)
	{
		datatofloat.data[i] = RxMessage.Data[i];
	}
	CAN_cntmeter = datatofloat.value;
	vaule_f = CAN_cntmeter + section_diff;

	cjson_can = cJSON_CreateObject();

	cjson_header = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson_header, "messageName", "METER_COUNTER_STATUS_IND");
	cJSON_AddStringToObject(cjson_header, "messageType", "MONITORING");
	cJSON_AddItemToObject(cjson_can, "header", cjson_header);

	cjson_payload = cJSON_CreateObject();
	cJSON_AddFalseToObject(cjson_payload, "isLateral");
	cJSON_AddStringToObject(cjson_payload, "unit", "meter");

	cJSON_AddNumberToObject(cjson_payload, "value", vaule_f);
	cJSON_AddItemToObject(cjson_can, "payload", cjson_payload);

	TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
	TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
	char *enter = "\n\0";
	strncat(TCPSendBuff, enter, 2);
	CAN_Cnt = strlen(TCPSendBuff);
	RecTime = GetTickCount();
#if DEBUG
	printf("RecTime:%d, vaule_f:%f, CAN_cntmeter:%f, section_diff:%f, total_diff:%f\r\n", RecTime, vaule_f, CAN_cntmeter, section_diff, total_diff);
	printf("CANBuff_cnt:%d \r\n", CAN_Cnt);
	printf("TCPSendBuff:%s \r\n", TCPSendBuff);

	// printf("NubStrBuff:%s \r\n", NubStrBuff);
#endif
	/*-----------------
	-------------------
	-------------------发送函数请放这（发送TCPSendBuff）
													-------------------
													-------------------
													-----------------*/
	sendToApp(TCPSendBuff);

	vaule_f = CAN_cntmeter + total_diff;
	// vaule = FloatToString(vaule_f);
	cJSON_ReplaceItemInObject(cjson_header, "messageName", cJSON_CreateString("TOTAL_METER_COUNTER_STATUS_IND"));
	cJSON_ReplaceItemInObject(cjson_payload, "value", cJSON_CreateNumber(vaule_f));

	TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
	TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
	strncat(TCPSendBuff, enter, 2);
	CAN_Cnt = strlen(TCPSendBuff);
	sendToApp(TCPSendBuff);
#if DEBUG
	printf("CANBuff_cnt:%d \r\n", CAN_Cnt);

	printf("TCPSendBuff:%s \r\n", TCPSendBuff);
#endif

	cJSON_Delete(cjson_can); // 释放内存
	cJSON_free(TCPSendBuff);
}

/**
 * @description  : CMSG_METERCNT1VALUE编码，小车压力状态显示
 * @return        {*}
 */
void CMSG_ROVVERPRESSURE_CODE(void)
{
	cJSON *cjson_can = NULL;
	cJSON *cjson_header = NULL;
	cJSON *cjson_payload = NULL;
	char *TCPSendBuff = NULL;
	DataTouInt16_t datatoint16_t;
	char cPressureState;
	for (uint8_t i = 0; i < 2; i++)
	{
		datatoint16_t.data[i] = RxMessage.Data[i];
	}

	cPressureState = RxMessage.Data[2];

	cjson_can = cJSON_CreateObject();

	cjson_header = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson_header, "messageName", "UPDATE_VALUE");
	cJSON_AddStringToObject(cjson_header, "messageType", "IPEK_CHINA_GUI");
	cJSON_AddItemToObject(cjson_can, "header", cjson_header);

	cjson_payload = cJSON_CreateObject();

	cJSON_AddStringToObject(cjson_payload, "what", "pressureStatus");
	if (cPressureState == 00)
		cJSON_AddStringToObject(cjson_payload, "value", "ok");
	else if ((cPressureState == 04) || (cPressureState == 06))
	{
		cJSON_AddStringToObject(cjson_payload, "value", "warning");
	}
	else
		cJSON_AddStringToObject(cjson_payload, "value", "critical");

	cJSON_AddItemToObject(cjson_can, "payload", cjson_payload);

	TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
	TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
	char *enter = "\n\0";
	strncat(TCPSendBuff, enter, 2);
	CAN_Cnt = strlen(TCPSendBuff);
	#if DEBUGSEND
		SEND_DEBUG_INFO("*************CMSG_ROVVERPRESSURE_CODE**************\r\n");
		SEND_DEBUG_INFO("压力状态CAN指令解析成功 \r\n");
		SEND_DEBUG_INFO("***************************************************\r\n");
	#endif
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

	cJSON_ReplaceItemInObject(cjson_payload, "what", cJSON_CreateString("pressureInMbar"));
	cJSON_ReplaceItemInObject(cjson_payload, "value", cJSON_CreateNumber(datatoint16_t.value));

	TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
	TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
	strncat(TCPSendBuff, enter, 2);
	CAN_Cnt = strlen(TCPSendBuff);
	sendToApp(TCPSendBuff);
#if DEBUG
	printf("CANBuff_cnt:%d \r\n", CAN_Cnt);

	printf("TCPSendBuff:%s \r\n", TCPSendBuff);
#endif

	cJSON_Delete(cjson_can); // 释放内存
	cJSON_free(TCPSendBuff);
}

/**
 * @description  : CMSG_METERCNT1VALUE编码，小车当前倾斜角度，侧翻状态报警
 * @return        {*}
 */
void CMSG_INCLINATIONXDEG_CODE(void)
{
	cJSON *cjson_can = NULL;
	cJSON *cjson_header = NULL;
	cJSON *cjson_payload = NULL;
	char *TCPSendBuff = NULL;
	DataToFloat datatofloat;
	char scState;
	for (uint8_t i = 0; i < 4; i++)
	{
		datatofloat.data[i] = RxMessage.Data[i];
	}

	scState = RxMessage.Data[4];

	cjson_can = cJSON_CreateObject();

	cjson_header = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson_header, "messageName", "UPDATE_VALUE");
	cJSON_AddStringToObject(cjson_header, "messageType", "IPEK_CHINA_GUI");
	cJSON_AddItemToObject(cjson_can, "header", cjson_header);

	cjson_payload = cJSON_CreateObject();

	cJSON_AddStringToObject(cjson_payload, "what", "crawlerOver");
	if (scState == 00)
		cJSON_AddStringToObject(cjson_payload, "value", "ok");
	else if ((scState == 01) || (scState == 02))
	{
		cJSON_AddStringToObject(cjson_payload, "value", "critical");
		if (cruise_flag == 1)
		{
			SendCrawlerSpeedValue(0, 0, 0);
		}
	}

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

	cJSON_ReplaceItemInObject(cjson_payload, "what", cJSON_CreateString("crawlerAngleInDegrees"));
	cJSON_ReplaceItemInObject(cjson_payload, "value", cJSON_CreateNumber(datatofloat.value * -1));
	TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
	TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
	strncat(TCPSendBuff, enter, 2);
	CAN_Cnt = strlen(TCPSendBuff);
	sendToApp(TCPSendBuff);

#if DEBUG
	printf("CANBuff_cnt:%d \r\n", CAN_Cnt);

	printf("TCPSendBuff:%s \r\n", TCPSendBuff);
#endif

	if (((scState == 01) || (scState == 02)) && (cruise_flag == 1))
	{
		cruise_flag = 0;
		cJSON_ReplaceItemInObject(cjson_payload, "what", cJSON_CreateString("cruiseControlStatus"));
		cJSON_ReplaceItemInObject(cjson_payload, "value", cJSON_CreateBool(0));
		TCPSendBuff = cJSON_PrintUnformatted(cjson_can);
		TCPSendBuff = realloc(TCPSendBuff, strlen(TCPSendBuff) + 2);
		strncat(TCPSendBuff, enter, 2);
		CAN_Cnt = strlen(TCPSendBuff);
		sendToApp(TCPSendBuff);
#if DEBUG
		printf("CANBuff_cnt:%d \r\n", CAN_Cnt);

		printf("TCPSendBuff:%s \r\n", TCPSendBuff);
#endif
	}

	cJSON_Delete(cjson_can); // 释放内存
	cJSON_free(TCPSendBuff);
}

/**
 * @description  : CMSG_METERCNT1VALUE编码，当前小车温度显示
 * @return        {*}
 */
void CMSG_ROVVERTEMP_CODE(void)
{
	cJSON *cjson_can = NULL;
	cJSON *cjson_header = NULL;
	cJSON *cjson_payload = NULL;
	char *TCPSendBuff = NULL;
	char scTemp;

	scTemp = RxMessage.Data[0];

	cjson_can = cJSON_CreateObject();

	cjson_header = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson_header, "messageName", "UPDATE_VALUE");
	cJSON_AddStringToObject(cjson_header, "messageType", "IPEK_CHINA_GUI");
	cJSON_AddItemToObject(cjson_can, "header", cjson_header);

	cjson_payload = cJSON_CreateObject();

	cJSON_AddNumberToObject(cjson_payload, "value", scTemp);

	cJSON_AddStringToObject(cjson_payload, "what", "temperatureInCelsius");
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
 * @description  : CMSG_METERCNT1VALUE编码，离合器状态
 * @return        {*}
 */
void CMSG_CLUTCHSTATE_CODE(void)
{
	cJSON *cjson_can = NULL;
	cJSON *cjson_header = NULL;
	cJSON *cjson_payload = NULL;
	char *TCPSendBuff = NULL;
	char clutchStatus;

	clutchStatus = RxMessage.Data[0];

	cjson_can = cJSON_CreateObject();

	cjson_header = cJSON_CreateObject();

	cJSON_AddStringToObject(cjson_header, "messageType", "IPEK_CHINA_GUI");
	cJSON_AddStringToObject(cjson_header, "messageName", "UPDATE_VALUE");

	cJSON_AddItemToObject(cjson_can, "header", cjson_header);

	cjson_payload = cJSON_CreateObject();

	cJSON_AddStringToObject(cjson_payload, "what", "clutchStatus");
	cJSON_AddBoolToObject(cjson_payload, "value", clutchStatus);

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

void NODEID_SEND_CODE(void)
{
	if ((RxMessage.Data[0] == 0x03) && (RxMessage.Data[7] == 0x03) && (RxMessage.Data[4] == 0x7B))
	{
		cJSON *cjson_can = NULL;
		cJSON *cjson_header = NULL;
		cJSON *cjson_payload = NULL;
		char *TCPSendBuff = NULL;

		cjson_can = cJSON_CreateObject();

		cjson_header = cJSON_CreateObject();

		cJSON_AddStringToObject(cjson_header, "messageType", "IPEK_CHINA_GUI");
		cJSON_AddStringToObject(cjson_header, "messageName", "UPDATE_VALUE");

		cJSON_AddItemToObject(cjson_can, "header", cjson_header);

		cjson_payload = cJSON_CreateObject();

		cJSON_AddStringToObject(cjson_payload, "what", "autoAngleMainLightsPossible");
		cJSON_AddBoolToObject(cjson_payload, "value", 1);

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
}

/**
 * @description  : CAN→TCP 		使用void Scheduler_Code(void)		此函数在can.c中的void CAN_RX_IRQHandler(void)接收中断调用，
 *								添加新的编码需要根据CANID添加case和相应的编码函数
 * @return        {*}
 */
#define nodeID 0x103
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
#if DEBUG
	printf("CANID:%x \r\n", CANID);
#endif
	// 接收成功
	MESSAGEID++;
	switch (CANID)
	{
	case nodeID:
	{
		NODEID_SEND_CODE();

		break;
	}
	case CMSG_METERCNT1VALUE:
	{
		CMSG_METERCNT1VALUE_CODE();

		break;
	}
	case CMSG_ROVVERPRESSURE:
	{
		CMSG_ROVVERPRESSURE_CODE();

		break;
	}
	case CMSG_ROVVERTEMP:
	{
		CMSG_ROVVERTEMP_CODE();

		break;
	}
	case CMSG_INCLINATIONXDEG:
	{
		CMSG_INCLINATIONXDEG_CODE();
		break;
	}
	case CMSG_CLUTCHSTATE:
	{
		CMSG_CLUTCHSTATE_CODE();
		break;
	}
	case CMSG_LIFTPOSITION_ELEVATOR:
	{
		CMSG_LIFTPOSITION_ELEVATOR_CODE();
		break;
	}
	default:
	{

#if DEBUG
		// printf("CAN指令错误\r\n");
#endif

	#if DEBUGSEND
		SEND_DEBUG_INFO("**************************CANDECODEERROR***********\r\n");
		SEND_DEBUG_INFO("无此ID,CANID:%x \r\n", CANID);
		SEND_DEBUG_INFO("***************************************************\r\n");
	#endif
//		MESSAGEID--;
		break;
	}
	}

}
//{"header":{"messageId":6,"messageName":"UPDATE_VALUE","messageType":"IPEK_CHINA_GUI"},"payload":{"value":"50%","what":"laserIntensity"}}
// "{\"header\":{\"messageId\":102,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":{\"angle\":57,\"power\":1},\"what\":\"roverJoystick\"}}"	//右侧操纵杆（控制小车）
//"{\"header\":{\"messageId\":5,\"messageName\":\"UPDATE_VALUE\",\"messageType\":\"IPEK_CHINA_GUI\"},\"payload\":{\"value\":1,\"what\":\"auxiliaryLights\"}}"	//
// 55 01 00 00 00 00 00 00 00 00 04 58 39 08 40 //CAN数据