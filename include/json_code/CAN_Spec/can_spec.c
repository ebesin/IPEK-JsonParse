/*
 * @Author       : iPEK
 * @Date         : 2023-03-07
 * @LastEditTime : 2023-03-10
 * @Description  : CAN解码、发送相关C文件，主要是配合上位机的CAN指令发送，文件内容已封装成材料中提供的内容
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */

#include "can_spec.h"
#include <stdio.h>
MessageId CANMessageId;
CantxMsg CanTMsg;
CanIDMsg CanToWiFiID;
CanIDMsg WiFiToCanID;

CanRxMsg RxMessage;
uint16_t CAN_Cnt;

#define EXT_MSG 0x00

/**
 * @description  : can发送一组数据，根据结构体内容发送
 * @param         {CantxMsg*} txMessage:CAN消息发送结构体
 * @return        {*}返回0：成功发送，1：发送失败
 */
uint8_t UDPSendBuff[20] = {0};

uint8_t CanSendMsg(CantxMsg *txMessage)
{

  CanTxMsg TxMessage;
  uint16_t WiFiToCanBuff_Cnt = 0;
  if (txMessage->MsgID > 0x7FF) // ��չID
  {
    TxMessage.ExtId = txMessage->MsgID;
    TxMessage.StdId = 0;
    TxMessage.IDE = CAN_ID_EXT;
  }
  else
  {
    TxMessage.ExtId = 0;
    TxMessage.StdId = txMessage->MsgID;
    TxMessage.IDE = CAN_ID_STD;
  }

  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.DLC = txMessage->DataLength;

  WiFiToCanID.StdId.id = TxMessage.StdId;

  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.StdId.data[0];
  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.StdId.data[1];
  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.StdId.data[2];
  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.StdId.data[3];

  WiFiToCanID.ExtId.id = TxMessage.ExtId;
  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.ExtId.data[0];
  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.ExtId.data[1];
  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.ExtId.data[2];
  UDPSendBuff[WiFiToCanBuff_Cnt++] = WiFiToCanID.ExtId.data[3];

  UDPSendBuff[WiFiToCanBuff_Cnt++] = TxMessage.IDE;
  UDPSendBuff[WiFiToCanBuff_Cnt++] = TxMessage.RTR;
  UDPSendBuff[WiFiToCanBuff_Cnt++] = TxMessage.DLC;

  for (uint8_t i = 0; i < TxMessage.DLC; i++)
  {
    UDPSendBuff[WiFiToCanBuff_Cnt++] = txMessage->Data.c[i];
  }
  for (uint8_t i = 0; i < WiFiToCanBuff_Cnt; i++)
  {
#if DEBUG
    printf("%x ", UDPSendBuff[i]);
#endif
  }

#if DEBUG
  printf("\r\n");
#endif
  sendToRobot(UDPSendBuff, WiFiToCanBuff_Cnt);
  for (uint8_t i = 0; i < TxMessage.DLC; i++)
  {
    txMessage->Data.c[i] = 0;
  }
}

//=============================================================================
void SendClutchOpen(CHAR cOnOff)
{
  CanTMsg.MsgID = CMSG_SETCTRLSOURCEINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 1;
  CanTMsg.Data.c[1] = cOnOff;
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = CMSG_CRAWLERCLUTCH;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 1;
  CanTMsg.Data.c[1] = cOnOff;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendFastFocusEvent(SCHAR scV1)
{
  CanTMsg.MsgID = CMSG_FOCUSSPEED_CTRLINT; // 0x154=340
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scV1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendFullStop(void)
{
  CanTMsg.MsgID = 2116;
  CanTMsg.DataLength = 8;
  CanTMsg.Data.c[7] = 1;
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = 996;
  CanTMsg.DataLength = 3;
  CanTMsg.Data.c[0] = 0;
  CanTMsg.Data.c[1] = 0x13;
  CanTMsg.Data.c[2] = 1;
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = 2116;
  CanTMsg.DataLength = 8;
  CanTMsg.Data.c[7] = 0;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendReelFunctionCodeEvent(SCHAR scMode)
{
  CanTMsg.MsgID = CMSG_REELFUNCTIONCODE_CTRLINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scMode;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendautoAngleMainLightsValueInDegrees(SCHAR scV1)
{
  CanTMsg.MsgID = 1604;
  CanTMsg.DataLength = 3;
  CanTMsg.Data.c[0] = 2;
  CanTMsg.Data.c[1] = 6;
  CanTMsg.Data.c[2] = scV1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendautoAngleMainLightsStatus(UCHAR ucMode)
{
  CanTMsg.MsgID = 1604;
  CanTMsg.DataLength = 3;
  CanTMsg.Data.c[0] = 2;
  CanTMsg.Data.c[1] = 1;
  CanTMsg.Data.c[2] = ucMode;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendMainLightEvent(SCHAR scV1)
{
  CanTMsg.MsgID = 1604;
  CanTMsg.DataLength = 3;
  CanTMsg.Data.c[0] = 2;
  CanTMsg.Data.c[1] = 3;
  CanTMsg.Data.c[2] = scV1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendlowBeamMainLightsEvent(SCHAR scV1)
{
  CanTMsg.MsgID = 1604;
  CanTMsg.DataLength = 3;
  CanTMsg.Data.c[0] = 2;
  CanTMsg.Data.c[1] = 2;
  CanTMsg.Data.c[2] = scV1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendStartupCMD(void)
{
  CanTMsg.MsgID = 2036;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = 1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendPowerOffCMD(void)
{
  CanTMsg.MsgID = 2036;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = 0;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendPowerModeNormal(void)
{
  CanTMsg.MsgID = CMSG_POWERMODE_CTRLINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 1;
  CanTMsg.Data.c[1] = 1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendClutch(CHAR cOnOff)
{
  CanTMsg.MsgID = CMSG_SETCTRLSOURCEINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = 0;
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = CMSG_CRAWLERCLUTCH;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = cOnOff;
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendClutchGear(CHAR cGear, CHAR cSecure)
{

  CanTMsg.MsgID = CMSG_CRAWLERCLUTCH;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = cGear; // 0,1,2
  CanTMsg.Data.c[1] = cSecure;
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendCrawlerSpeedValue(SCHAR scV1, SCHAR scV2, UCHAR ucMode)
{
  CanTMsg.MsgID = CMSG_MOTORSPEED_CTRLINT;
  CanTMsg.DataLength = 3;
  CanTMsg.Data.c[0] = scV1;   // 2
  CanTMsg.Data.c[1] = scV2;   // 1
  CanTMsg.Data.c[2] = ucMode; // 0 = Std, 1 = CruiseControl

  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendLiftSpeedValue(SCHAR scLiftSpeed)
{
  CanTMsg.MsgID = CMSG_CRAWLERLIFT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = scLiftSpeed; // 0...100
  CanTMsg.Data.c[1] = 0;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendReelSpeed(SCHAR scValue)
{
  CanTMsg.MsgID = CMSG_REELSPEED_CTRLINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scValue;
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendReelPower(SCHAR scValue)
{
  CanTMsg.MsgID = CMSG_REELPOWER_CTRLINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scValue;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendCameraJoystickEvent(SCHAR scV1, SCHAR scV2)
{
  CanTMsg.MsgID = CMSG_TILTSPEED_CTRLINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scV2 * (-1);
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = CMSG_PANSPEED_CTRLINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scV1 * (-1);
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendCameraZoomEvent(SCHAR scV)
{
  CanTMsg.MsgID = CMSG_ZOOMSPEED_CTRLINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scV;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendFocusEvent(SCHAR scV1)
{
  CanTMsg.MsgID = CMSG_FOCUSPULS_CTRLINT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scV1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SetAutoFocus(void)
{
  CanTMsg.MsgID = CMSG_SONYFUNCTION;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 55; // command
  CanTMsg.Data.c[1] = 0;  // parameter
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SetManualFocus(void)
{
  CanTMsg.MsgID = CMSG_SONYFUNCTION;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 54; // command
  CanTMsg.Data.c[1] = 0;  // parameter
  CanSendMsg(&CanTMsg);
}

//=============================================================================
// void SendCameraLightEvent(SCHAR scV1)
//{
//  CanTMsg.MsgID      = CMSG_CAMERALIGHT_CCU1;
//  CanTMsg.DataLength = 1;
//  CanTMsg.DataLength|= EXT_MSG;
//  CanTMsg.Data.c[0]  = scV1;
//  CanSendMsg( &CanTMsg );
//}

////=============================================================================
void SendExtraLightEvent(SCHAR scV1)
{
  CanTMsg.MsgID = CMSG_EXTRALIGHT;
  CanTMsg.DataLength = 1;
  CanTMsg.DataLength |= EXT_MSG;
  CanTMsg.Data.c[0] = scV1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
// Ŀǰû�к����������Զ�������,CMSG_LASERPOINTER  =  132 ,
void SendLaserPointEvent(SCHAR scV1)
{
  CanTMsg.MsgID = CMSG_LASERPOINTER;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = scV1;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
//===spec��û�У��Զ�������==============================================================
void SetLocatorConfigEvent(SCHAR uVal, SCHAR uVa2)
{
  CanTMsg.MsgID = CMSG_SETLOCATORCONFIG_CTRLINT; // 612=0x264
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = uVal;
  CanTMsg.Data.c[1] = uVa2;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendCameraMacroCmd(CHAR cMode, CHAR cMacro)
{
  CanTMsg.MsgID = CMSG_MACRO_CMD_CTRLINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 1;
  CanTMsg.Data.c[1] = cMacro; // number 1...x
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendCameraHomePositionEvent(void)
{
  CanTMsg.MsgID = CMSG_SETPANPOS_CTRLINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 0;
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = CMSG_SETTILTPOS_CTRLINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 0;
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = CMSG_MACRO_CMD_CTRLINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = 1;
  CanTMsg.Data.c[1] = 5;
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendCameraPanTiltPositionEvent(USHORT usPan, USHORT usTilt)
{
  CanTMsg.MsgID = CMSG_SETPANPOS_CTRLINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = usPan;
  CanSendMsg(&CanTMsg);

  CanTMsg.MsgID = CMSG_SETTILTPOS_CTRLINT;
  CanTMsg.DataLength = 2;
  CanTMsg.Data.c[0] = usTilt;
  CanSendMsg(&CanTMsg);
}
//=============================================================================
void SendBackViewCameraSwitchEvent(UCHAR ucVal)
{
  CanTMsg.MsgID = CMSG_RFCAMERA;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = ucVal; // 0,1,2,..
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendSystemFullStopEvent(void)
{
  CanTMsg.MsgID = CMSG_CONTROL_CCU1;
  CanTMsg.DataLength = 4;
  CanTMsg.DataLength |= EXT_MSG;
  CanTMsg.Data.c[0] = 0;
  CanTMsg.Data.c[1] = 19;
  CanTMsg.Data.c[2] = 1;
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendAutoHighBeamMainLightsValueInPercentEvent(UCHAR ucVal)
{
  CanTMsg.MsgID = CMSG_CAMERALIGHT;
  CanTMsg.DataLength = 1;
  CanTMsg.Data.c[0] = ucVal;
  CanSendMsg(&CanTMsg);
}

//=============================================================================
void SendChangeMeterCounterValueEvent(UCHAR uVal1, UCHAR uVal2, UCHAR uVal3, UCHAR uVal4)
{
  CanTMsg.MsgID = CMSG_SETMETERCNT1VALUE_CTRLINT;
  CanTMsg.DataLength = 4;
  CanTMsg.Data.c[0] = uVal1;
  CanTMsg.Data.c[1] = uVal2;
  CanTMsg.Data.c[2] = uVal3;
  CanTMsg.Data.c[3] = uVal4;
  CanSendMsg(&CanTMsg);
}