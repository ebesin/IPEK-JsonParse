/*
 * @Author       : iPEK
 * @Date         : 2023-03-07
 * @LastEditTime : 2023-03-10
 * @Description  : CAN解码、发送相关H文件，主要是配合上位机的CAN指令发送，文件内�?�已封�?�成材料�?提供的内�?
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __CAN_SPEC_H
#define __CAN_SPEC_H

#include "../../socket_forward/my_socket.h"
#include "../../socket_forward/cDefine.h"

/**
 * @description  : CANID枚举
 */
typedef enum
{
  CMSG_NO_CANID = 0,
  CMSG_MOTORSPEED_CTRLINT = 20,
  CMSG_LIVESTATUS_CTRLINT = 36,
  CMSG_PANSPEED_CTRLINT = 52,
  CMSG_TILTSPEED_CTRLINT = 68,
  CMSG_ZOOMSPEED_CTRLINT = 84,
  CMSG_CAMERALIGHT = 100,
  CMSG_EXTRALIGHT = 116,
  CMSG_LASERPOINTER = 132,
  CMSG_CRAWLERCLUTCH = 148,
  CMSG_CRAWLERLIFT = 164,
  CMSG_REELSPEED_CTRLINT = 180,
  CMSG_REELPOWER_CTRLINT = 196,
  CMSG_SETCTRLSOURCEINT = 212,
  CMSG_SONYFUNCTION = 228,
  CMSG_PANASONICFUNCTION = 244,
  CMSG_SENDNODEID_CTRLINT = 260,
  CMSG_READNODEID_CTRLINT = 276,
  CMSG_FOCUSPULS_CTRLINT = 292,
  CMSG_REELFUNCTIONCODE_CTRLINT = 308,
  CMSG_EXTRALIGHT2 = 324,
  CMSG_FOCUSSPEED_CTRLINT = 340,
  CMSG_SATCTRLDATA_CTRLINT = 356,
  CMSG_APPINFO_CTRLINT = 372,
  CMSG_CAMERALIGHT2 = 388,
  CMSG_SENDATEXFLAG_CTRLINT = 420,
  CMSG_SENDSNR_CTRLINT = 468,
  CMSG_CAMERAFCBIX10INQ_CTRLINT = 484,
  CMSG_SETINSERTIONPOS_CTRLINT = 500,
  CMSG_SETPANPOS_CTRLINT = 516,
  CMSG_SETTILTPOS_CTRLINT = 532,
  CMSG_SETMETERCNT1VALUE_CTRLINT = 548,
  CMSG_SETMETERCNT2VALUE_CTRLINT = 564,
  CMSG_RFCAMERA = 580,
  CMSG_MODE_CMD_CTRLINT = 596,
  CMSG_SETLOCATORCONFIG_CTRLINT = 612,
  CMSG_SATREELSPEED_CTRLINT = 628,
  CMSG_FEEDERMOTORSPEED_CTRLINT = 644,
  CMSG_SATREELPOWER_CTRLINT = 660,
  CMSG_INSERTIONROLLSPEED_CTRLINT = 676,
  CMSG_INSERTIONPITCHSPEED_CTRLINT = 692,
  CMSG_SATREELFUNCTIONCODE_CTRLINT = 708,
  CMSG_SETMETERCNTSAT1VALUE_CTRLINT = 724,
  CMSG_SETSATCONTROLSTATE_CTRLINT = 740,
  CMSG_CAMERAINSERTIONMODE_CTRLINT = 756,
  CMSG_AUTOPILOT_CTRLINT = 772,
  CMSG_TEXTGENERATORTEXTCOLOR = 788,
  CMSG_3DCONTROL = 804,
  CMSG_SETMETERCNT3VALUE_CTRLINT = 820,
  CMSG_SETMETERCNT4VALUE_CTRLINT = 836,
  CMSG_CTRLPNLMODE_CTRLINT = 852,
  CMSG_EXTMTCNTOSDCONTROL = 868,
  CMSG_EXTMTCNTOSDVALUE = 884,
  CMSG_BRR_STATUS_CTRLINT = 916,
  CMSG_CONTROL_CTRLINT = 996,
  CMSG_STATUS_CTRLINT = 1012,
  CMSG_SETCNCMD_CTRLINT = 1158,
  CMSG_GETCNCMD_CTRLINT = 1174,
  CMSG_BLSETCMD_CTRLINT = 1163,
  CMSG_BLGETCMD_CTRLINT = 1179,
  CMSG_SWRESET_CTRLINT = 1153,
  CMSG_SWRESETACK_CTRLINT = 1169,
  CMSG_SENDSTATUS_CTRLINT = 1154,
  CMSG_READSTATUS_CTRLINT = 1170,
  CMSG_ANTIROLLAUTOSTOP_CTRLINT = 1556,
  CMSG_REELCTRLDATA_CTRLINT = 1572,
  CMSG_LASERCALIBRATION = 1588,
  CMSG_EXTND_LED_CTRL = 1604,
  CMSG_VIDEO_SWITCH_CTRLINT = 1684,
  CMSG_CAMERA_REQUEST_CTRLINT = 1812,
  CMSG_MACRO_CMD_CTRLINT = 1828,
  CMSG_POWERMODE_CTRLINT = 1844,
  CMSG_VIDEOSTATE_CTRLINT = 1860,
  CMSG_REQUEST_DATA_CTRLINT = 1924,
  CMSG_RESPONSE_DATA_CTRLINT = 1940,
  CMSG_ROVVERUIN = 18,
  CMSG_LIVESTATUS_ROVVER = 34,
  CMSG_SENDNODEID_ROVVER = 258,
  CMSG_READNODEID_ROVVER = 274,
  CMSG_SENDATEXFLAG_ROVVER = 418,
  CMSG_SENDSNR_ROVVER = 466,
  CMSG_CONTROL_ROVVER = 994,
  CMSG_STATUS_ROVVER = 1010,
  CMSG_SETCNCMD_ROVVER = 1094,
  CMSG_GETCNCMD_ROVVER = 1110,
  CMSG_BLSETCMD_ROVVER = 1099,
  CMSG_BLGETCMD_ROVVER = 1115,
  CMSG_SWRESET_ROVVER = 1089,
  CMSG_SWRESETACK_ROVVER = 1105,
  CMSG_SENDSTATUS_ROVVER = 1090,
  CMSG_READSTATUS_ROVVER = 1106,
  CMSG_CRAWLERIP12P5 = 1554,
  CMSG_ICAMERALIGHT = 1570,
  CMSG_IEXTRALIGHT = 1586,
  CMSG_IMOTOR = 1602,
  CMSG_MOTORREVOLUTION = 1618,
  CMSG_IPANMOTOR = 1634,
  CMSG_ROVVERTEMP = 1650,
  CMSG_ROVVERPRESSURE = 1666,
  CMSG_INCLINATIONY = 1682,
  CMSG_INCLINATIONX = 1698,
  CMSG_PANPOS = 1714,
  CMSG_INCLINATIONYDEG = 1730,
  CMSG_INCLINATIONXDEG = 1746,
  CMSG_ILIFTMOTORCURRENTREV = 1762,
  CMSG_CLUTCHSTATE = 1778,
  CMSG_FEEDERMOTORREVOLUION = 1794,
  CMSG_FEEDERMOTORCURRENT = 1810,
  CMSG_INSERTIONROLLPOS = 1826,
  CMSG_INSERTIONPITCHPOS = 1842,
  CMSG_PUSHRODEREVOLUTION = 1858,
  CMSG_IFEEDERMOTORCURRENTREV = 1874,
  CMSG_SATCAMERA_RESPONSE = 1890,
  CMSG_LIFTPOSITION_ROVVER = 1906,
  CMSG_REQUEST_DATA_ROVVER = 1922,
  CMSG_RESPONSE_DATA_ROVVER = 1938,
  CMSG_PRESSURE_2_ROVVER = 1954,
  CMSG_EXTRAMODUL_ROVVER = 1970,
  CMSG_MOTORSPEED_CTRLEXT = 22,
  CMSG_LIVESTATUS_CTRLEXT = 38,
  CMSG_PANSPEED_CTRLEXT = 54,
  CMSG_TILTSPEED_CTRLEXT = 70,
  CMSG_CRAWLERCLUTCH_CTRLEXT = 150,
  CMSG_CRAWLERLIFT_CTRLEXT = 166,
  CMSG_REELSPEED_CTRLEXT = 182,
  CMSG_REELPOWER_CTRLEXT = 198,
  CMSG_SETCTRLSOURCEEXT = 214,
  CMSG_SENDNODEID_CTRLEXT = 262,
  CMSG_READNODEID_CTRLEXT = 278,
  CMSG_REELFUNCTIONCODE_CTRLEXT = 310,
  CMSG_KRANFUNCTIONCODE_CTRLEXT = 326,
  CMSG_SENDSNR_CTRLEXT = 470,
  CMSG_SENDATEXFLAG_CTRLEXT = 422,
  CMSG_SETPANPOS_CTRLEXT = 518,
  CMSG_SETTILTPOS_CTRLEXT = 534,
  CMSG_SATREELSPEED_CTRLEXT = 550,
  CMSG_FEEDERMOTORSPEED_CTRLEXT = 566,
  CMSG_SATREELPOWER_CTRLEXT = 582,
  CMSG_SATREELFUNCTIONCODE_CTRLEXT = 598,
  CMSG_INSERTIONROLLSPEED_CTRLEXT = 614,
  CMSG_INSERTIONPITCHSPEED_CTRLEXT = 630,
  CMSG_CONTROL_CTRLEXT = 998,
  CMSG_STATUS_CTRLEXT = 1014,
  CMSG_SETCNCMD_CTRLEXT = 1222,
  CMSG_GETCNCMD_CTRLEXT = 1238,
  CMSG_BLSETCMD_CTRLEXT = 1227,
  CMSG_BLGETCMD_CTRLEXT = 1243,
  CMSG_SWRESET_CTRLEXT = 1217,
  CMSG_SWRESETACK_CTRLEXT = 1233,
  CMSG_SENDSTATUS_CTRLEXT = 1218,
  CMSG_READSTATUS_CTRLEXT = 1234,
  CMSG_REQUEST_DATA_CTRLEXT = 1926,
  CMSG_RESPONSE_DATA_CTRLEXT = 1942,
  CMSG_LIVESTATUS_SVPS = 33,
  CMSG_EX1ALARM_SVPS = 65,
  CMSG_SENDNODEID_SVPS = 257,
  CMSG_READNODEID_SVPS = 273,
  CMSG_SENDATEXFLAG_SVPS = 417,
  CMSG_SENDSNR_SVPS = 465,
  CMSG_HISTORY_RESPONSE_SVPS = 513,
  CMSG_ACTUAL_TIME_SVPS = 593,
  CMSG_ACTUAL_DATE_SVPS = 609,
  CMSG_UPDATE_TIME_SVPS = 625,
  CMSG_UPDATE_DATE_SVPS = 641,
  CMSG_CONTROL_SVPS = 993,
  CMSG_STATUS_SVPS = 1009,
  CMSG_SETCNCMD_SVPS = 1062,
  CMSG_GETCNCMD_SVPS = 1078,
  CMSG_BLSETCMD_SVPS = 1067,
  CMSG_BLGETCMD_SVPS = 1083,
  CMSG_SWRESET_SVPS = 1057,
  CMSG_SWRESETACK_SVPS = 1073,
  CMSG_SENDSTATUS_SVPS = 1058,
  CMSG_READSTATUS_SVPS = 1074,
  CMSG_SUPPLYUIOUT = 1553,
  CMSG_SUPPLYKOMPENSATION = 1569,
  CMSG_ISOLATIONERROR = 1585,
  CMSG_REQUEST_DATA_SVPS = 1921,
  CMSG_RESPONSE_DATA_SVPS = 1937,
  CMSG_LIVESTATUS_DSP = 41,
  CMSG_SENDNODEID_DSP = 265,
  CMSG_READNODEID_DSP = 281,
  CMSG_SENDATEXFLAG_DSP = 425,
  CMSG_SENDSNR_DSP = 473,
  CMSG_HISTORY_REQUEST_DSP = 521,
  CMSG_CONTROL_DSP = 1001,
  CMSG_STATUS_DSP = 1017,
  CMSG_SYNC_TIMEDATE_DSP = 649,
  CMSG_SETCNCMD_DSP = 1318,
  CMSG_GETCNCMD_DSP = 1334,
  CMSG_BLSETCMD_DSP = 1323,
  CMSG_BLGETCMD_DSP = 1339,
  CMSG_SWRESET_DSP = 1313,
  CMSG_SWRESETACK_DSP = 1329,
  CMSG_SENDSTATUS_DSP = 1314,
  CMSG_READSTATUS_DSP = 1330,
  CMSG_REQUEST_DATA_DSP = 1929,
  CMSG_RESPONSE_DATA_DSP = 1945,
  CMSG_LIVESTATUS_CAMERA = 35,
  CMSG_SENDNODEID_CAMERA = 259,
  CMSG_READNODEID_CAMERA = 275,
  CMSG_SENDATEXFLAG_CAMERA = 419,
  CMSG_SENDSNR_CAMERA = 467,
  CMSG_SENDCAMERALIGHTLIMIT = 291,
  CMSG_UPRIGHTPICCTRL_CAMERA = 979,
  CMSG_CONTROL_CAMERA = 995,
  CMSG_STATUS_CAMERA = 1011,
  CMSG_SETCNCMD_CAMERA = 1126,
  CMSG_GETCNCMD_CAMERA = 1142,
  CMSG_BLSETCMD_CAMERA = 1131,
  CMSG_BLGETCMD_CAMERA = 1147,
  CMSG_SWRESET_CAMERA = 1121,
  CMSG_SWRESETACK_CAMERA = 1137,
  CMSG_SENDSTATUS_CAMERA = 1122,
  CMSG_READSTATUS_CAMERA = 1138,
  CMSG_PANTILTSPEED = 1555,
  CMSG_LASERDATA_CAMERA = 1587,
  CMSG_EXTND_LED_RESPONSE = 1603,
  CMSG_CAMERATEMP = 1651,
  CMSG_CAMERAPRESSURE = 1667,
  CMSG_TILTPOS = 1715,
  CMSG_AUTOFOCUSSTATE = 1795,
  CMSG_CAMERA_RESPONSE = 1891,
  CMSG_REQUEST_DATA_CAMERA = 1923,
  CMSG_RESPONSE_DATA_CAMERA = 1939,
  CMSG_PRESSURE_2_CAMERA = 1955,
  CMSG_CAMERAFCBIX10STATE = 2019,
  CMSG_UIN_PTP70 = 28,
  CMSG_LIVESTATUS_PTP70 = 44,
  CMSG_SENDNODEID_PTP70 = 268,
  CMSG_READNODEID_PTP70 = 284,
  CMSG_SENDATEXFLAG_PTP70 = 428,
  CMSG_SENDSNR_PTP70 = 476,
  CMSG_CONTROL_PTP70 = 1004,
  CMSG_STATUS_PTP70 = 1020,
  CMSG_SWRESET_PTP70 = 1409,
  CMSG_SENDSTATUS_PTP70 = 1410,
  CMSG_SETCNCMD_PTP70 = 1414,
  CMSG_BLSETCMD_PTP70 = 1419,
  CMSG_SWRESETACK_PTP70 = 1425,
  CMSG_READSTATUS_PTP70 = 1426,
  CMSG_GETCNCMD_PTP70 = 1430,
  CMSG_BLGETCMD_PTP70 = 1435,
  CMSG_TEMP_PTP70 = 1660,
  CMSG_PRESSURE_PTP70 = 1676,
  CMSG_POSITION_PTP50 = 1692,
  CMSG_PANTILTPOS_PTP70 = 1724,
  CMSG_MACRO_STATUS_PTP70 = 1820,
  CMSG_CAMERA_RESPONSE_PTP70 = 1900,
  CMSG_SENDLOCATORCONFIG_PTP70 = 1916,
  CMSG_REQUEST_DATA_PTP70 = 1932,
  CMSG_RESPONSE_DATA_PTP70 = 1948,
  CMSG_LIVESTATUS_VIDEO = 40,
  CMSG_SENDNODEID_VIDEO = 264,
  CMSG_READNODEID_VIDEO = 280,
  CMSG_SENDATEXFLAG_VIDEO = 424,
  CMSG_CONTROL_VIDEO = 1000,
  CMSG_STATUS_VIDEO = 1016,
  CMSG_SETCNCMD_VIDEO = 1286,
  CMSG_GETCNCMD_VIDEO = 1302,
  CMSG_BLSETCMD_VIDEO = 1291,
  CMSG_BLGETCMD_VIDEO = 1307,
  CMSG_SWRESET_VIDEO = 1281,
  CMSG_SWRESETACK_VIDEO = 1297,
  CMSG_SENDSTATUS_VIDEO = 1282,
  CMSG_READSTATUS_VIDEO = 1298,
  CMSG_BURSTSYNCLEVEL_VIDEO = 1560,
  CMSG_REQUEST_DATA_VIDEO = 1928,
  CMSG_RESPONSE_DATA_VIDEO = 1944,
  CMSG_LIVESTATUS_REEL = 37,
  CMSG_SENDNODEID_REEL = 261,
  CMSG_READNODEID_REEL = 277,
  CMSG_UPDATE_METERCNT_REEL = 325,
  CMSG_METERCNT1VALUE = 341,
  CMSG_METERCNT2VALUE = 357,
  CMSG_METERCNT3VALUE = 373,
  CMSG_METERCNT4VALUE = 389,
  CMSG_AKKUVOLTAGE = 405,
  CMSG_SENDATEXFLAG_REEL = 421,
  CMSG_AKKUVOLTAGE_II = 437,
  CMSG_AKKUSTATE = 453,
  CMSG_SENDSNR_REEL = 469,
  CMSG_CONTROL_REEL = 997,
  CMSG_STATUS_REEL = 1013,
  CMSG_SETCNCMD_REEL = 1190,
  CMSG_GETCNCMD_REEL = 1206,
  CMSG_BLSETCMD_REEL = 1195,
  CMSG_BLGETCMD_REEL = 1211,
  CMSG_SWRESET_REEL = 1185,
  CMSG_SWRESETACK_REEL = 1201,
  CMSG_SENDSTATUS_REEL = 1186,
  CMSG_READSTATUS_REEL = 1202,
  CMSG_SUPPLYUIOUT_REEL = 1557,
  CMSG_METERCOUNTEVENT_REEL = 1573,
  CMSG_TEMPERATURE_REEL = 1653,
  CMSG_REQUEST_DATA_REEL = 1925,
  CMSG_RESPONSE_DATA_REEL = 1941,
  CMSG_LIVESTATUS_SVASAT120 = 43,
  CMSG_SENDNODEID_SVASAT120 = 267,
  CMSG_READNODEID_SVASAT120 = 283,
  CMSG_UPDATE_METERCNT_SVASAT120 = 331,
  CMSG_METERCNTSAT1VALUE = 347,
  CMSG_SENDATEXFLAG_SAT = 427,
  CMSG_SENDSNR_SAT = 475,
  CMSG_CONTROL_SVASAT120 = 1003,
  CMSG_STATUS_SVASAT120 = 1019,
  CMSG_SETCNCMD_SVASAT120 = 1382,
  CMSG_GETCNCMD_SVASAT120 = 1398,
  CMSG_BLSETCMD_SVASAT120 = 1387,
  CMSG_BLGETCMD_SVASAT120 = 1403,
  CMSG_SWRESET_SVASAT120 = 1377,
  CMSG_SWRESETACK_SVASAT120 = 1393,
  CMSG_SENDSTATUS_SVASAT120 = 1378,
  CMSG_READSTATUS_SVASAT120 = 1394,
  CMSG_REQUEST_DATA_SVASAT120 = 1931,
  CMSG_RESPONSE_DATA_SVASAT120 = 1947,
  CMSG_LIVESTATUS_TEXTG = 39,
  CMSG_PANSPEED_TEXTG = 55,
  CMSG_TILTSPEED_TEXTG = 71,
  CMSG_SENDNODEID_TEXTG = 263,
  CMSG_READNODEID_TEXTG = 279,
  CMSG_SETPANPOS_SPEED_TEXTG = 487,
  CMSG_SETTILTPOS_SPEED_TEXTG = 503,
  CMSG_SENDATEXFLAG_TEXTG = 423,
  CMSG_SETPANPOS_TEXTG = 519,
  CMSG_SETTILTPOS_TEXTG = 535,
  CMSG_SETMETERCNT1VALUE_TEXTG = 551,
  CMSG_SETMETERCNT2VALUE_TEXTG = 567,
  CMSG_SETMETERCNTSAT1VALUE_TEXTG = 583,
  CMSG_ACTUALTIME_TEXTG = 599,
  CMSG_ACTUALDATE_TEXTG = 615,
  CMSG_LIFTCMDHEIGHT_TEXTG = 631,
  CMSG_LIFTCMDOFFSET_TEXTG = 647,
  CMSG_AUTOPILOT_TEXTG = 775,
  CMSG_CONTROL_TEXTGEN = 999,
  CMSG_STATUS_TEXTGEN = 1015,
  CMSG_SETCNCMD_TEXTG = 1254,
  CMSG_GETCNCMD_TEXTG = 1270,
  CMSG_BLSETCMD_TEXTG = 1259,
  CMSG_BLGETCMD_TEXTG = 1275,
  CMSG_SWRESET_TEXTG = 1249,
  CMSG_SWRESETACK_TEXTG = 1265,
  CMSG_SENDSTATUS_TEXTG = 1250,
  CMSG_READSTATUS_TEXTG = 1266,
  CMSG_REQUEST_DATA_TEXTGEN = 1927,
  CMSG_RESPONSE_DATA_TEXTGEN = 1943,
  CMSG_FW250RSVUIN = 26,
  CMSG_LIVESTATUS_FW250RSV = 42,
  CMSG_SENDNODEID_FW250RSV = 266,
  CMSG_READNODEID_FW250RSV = 282,
  CMSG_SENDATEXFLAG_FW250RSV = 426,
  CMSG_CONTROL_FW250RSV = 1002,
  CMSG_STATUS_FW250RSV = 1018,
  CMSG_SETCNCMD_FW250RSV = 1350,
  CMSG_GETCNCMD_FW250RSV = 1366,
  CMSG_BLSETCMD_FW250RSV = 1355,
  CMSG_BLGETCMD_FW250RSV = 1371,
  CMSG_SWRESET_FW250RSV = 1345,
  CMSG_SWRESETACK_FW250RSV = 1361,
  CMSG_SENDSTATUS_FW250RSV = 1346,
  CMSG_READSTATUS_FW250RSV = 1362,
  CMSG_FW250RSVIP12P5 = 1562,
  CMSG_FW250RSVICAMERALIGHT = 1578,
  CMSG_FW250RSVIEXTRALIGHT = 1594,
  CMSG_FW250RSVIEXTRALIGHT2 = 1610,
  CMSG_FW250RSVIPANMOTOR = 1642,
  CMSG_FW250RSVTEMP = 1658,
  CMSG_FW250RSVPRESSURE = 1674,
  CMSG_FW250RSVPANPOS = 1722,
  CMSG_REQUEST_DATA_FW250RSV = 1930,
  CMSG_RESPONSE_DATA_FW250RSV = 1946,
  CMSG_PRESSURE_2_FW250RSV = 1962,
  CMSG_LIVESTATUS_TOOL = 46,
  CMSG_SENDNODEID_TOOL = 270,
  CMSG_READNODEID_TOOL = 286,
  CMSG_SENDATEXFLAG_TOOL = 430,
  CMSG_SENDSNR_TOOL = 478,
  CMSG_CONTROL_TOOL = 1006,
  CMSG_STATUS_TOOL = 1022,
  CMSG_SETCNCMD_TOOL = 1478,
  CMSG_GETCNCMD_TOOL = 1494,
  CMSG_BLSETCMD_TOOL = 1483,
  CMSG_BLGETCMD_TOOL = 1499,
  CMSG_SWRESET_TOOL = 1473,
  CMSG_SWRESETACK_TOOL = 1489,
  CMSG_SENDSTATUS_TOOL = 1474,
  CMSG_READSTATUS_TOOL = 1490,
  CMSG_REQUEST_DATA_TOOL = 1934,
  CMSG_RESPONSE_DATA_TOOL = 1950,
  CMSG_LIVESTATUS_EXTMODULE = 45,
  CMSG_SENDNODEID_EXTMODULE = 269,
  CMSG_READNODEID_EXTMODULE = 285,
  CMSG_SENDATEXFLAG_EXTMODULE = 429,
  CMSG_SENDSNR_EXTMODULE = 477,
  CMSG_CONTROL_EXTMODULE = 1005,
  CMSG_STATUS_EXTMODULE = 1021,
  CMSG_SETCNCMD_EXTMODULE = 1446,
  CMSG_GETCNCMD_EXTMODULE = 1462,
  CMSG_BLSETCMD_EXTMODULE = 1451,
  CMSG_BLGETCMD_EXTMODULE = 1467,
  CMSG_SWRESET_EXTMODULE = 1441,
  CMSG_SWRESETACK_EXTMODULE = 1457,
  CMSG_SENDSTATUS_EXTMODULE = 1442,
  CMSG_READSTATUS_EXTMODULE = 1458,
  CMSG_FOLDERMOTPOSITIONS_EXTMODULE = 1661,
  CMSG_REQUEST_DATA_EXTMODULE = 1933,
  CMSG_RESPONSE_DATA_EXTMODULE = 1949,
  CMSG_MESUREMENT1_EXTMODULE = 1629,
  CMSG_LIVESTATUS_PSREEL = 2080,
  CMSG_SENDNODEID_PSREEL = 2304,
  CMSG_READNODEID_PSREEL = 2320,
  CMSG_SENDATEXFLAG_PSREEL = 2464,
  CMSG_SENDSNR_PSREEL = 2512,
  CMSG_LEADINGPOSITION_PSREEL = 2864,
  CMSG_BRR_STATUS_PSREEL = 2960,
  CMSG_CONTROL_PSREEL = 3040,
  CMSG_STATUS_PSREEL = 3056,
  CMSG_SETCNCMD_PSREEL = 3078,
  CMSG_GETCNCMD_PSREEL = 3094,
  CMSG_BLSETCMD_PSREEL = 3083,
  CMSG_BLGETCMD_PSREEL = 3099,
  CMSG_SWRESET_PSREEL = 3073,
  CMSG_SWRESETACK_PSREEL = 3089,
  CMSG_SENDSTATUS_PSREEL = 3074,
  CMSG_READSTATUS_PSREEL = 3090,
  CMSG_REELCTRLDATA_PSREEL = 3616,
  CMSG_PSREELTEMP = 3696,
  CMSG_POWERMODE_PSREEL = 3888,
  CMSG_REQUEST_DATA_PSREEL = 3968,
  CMSG_RESPONSE_DATA_PSREEL = 3984,
  CMSG_LIVESTATUS_ELEVATOR = 2081,
  CMSG_SENDNODEID_ELEVATOR = 2305,
  CMSG_READNODEID_ELEVATOR = 2321,
  CMSG_SENDATEXFLAG_ELEVATOR = 2465,
  CMSG_SENDSNR_ELEVATOR = 2513,
  CMSG_CONTROL_ELEVATOR = 3041,
  CMSG_STATUS_ELEVATOR = 3057,
  CMSG_SETCNCMD_ELEVATOR = 3110,
  CMSG_GETCNCMD_ELEVATOR = 3126,
  CMSG_BLSETCMD_ELEVATOR = 3115,
  CMSG_BLGETCMD_ELEVATOR = 3131,
  CMSG_SWRESET_ELEVATOR = 3105,
  CMSG_SWRESETACK_ELEVATOR = 3121,
  CMSG_SENDSTATUS_ELEVATOR = 3106,
  CMSG_READSTATUS_ELEVATOR = 3122,
  CMSG_ELEVATORTEMP = 3697,
  CMSG_ELEVATORPRESSURE = 3713,
  CMSG_ELEVATORIRDISTANCE = 3729,
  CMSG_LIFTPOSITION_ELEVATOR = 3953,
  CMSG_REQUEST_DATA_ELEVATOR = 3969,
  CMSG_RESPONSE_DATA_ELEVATOR = 3985,
  CMSG_LIFTANGLE_ELEVATOR = 4001,
  CMSG_LIVESTATUS_EXTRALIGHT = 2082,
  CMSG_SENDNODEID_EXTRALIGHT = 2306,
  CMSG_READNODEID_EXTRALIGHT = 2322,
  CMSG_SENDATEXFLAG_EXTRALIGHT = 2466,
  CMSG_SENDSNR_EXTRALIGHT = 2514,
  CMSG_CONTROL_EXTRALIGHT = 3042,
  CMSG_STATUS_EXTRALIGHT = 3058,
  CMSG_SETCNCMD_EXTRALIGHT = 3142,
  CMSG_GETCNCMD_EXTRALIGHT = 3158,
  CMSG_BLSETCMD_EXTRALIGHT = 3147,
  CMSG_BLGETCMD_EXTRALIGHT = 3163,
  CMSG_SWRESET_EXTRALIGHT = 3137,
  CMSG_SWRESETACK_EXTRALIGHT = 3153,
  CMSG_SENDSTATUS_EXTRALIGHT = 3138,
  CMSG_READSTATUS_EXTRALIGHT = 3154,
  CMSG_EXTRALIGHTTEMP = 3698,
  CMSG_EXTRALIGHTPRESSURE = 3714,
  CMSG_REQUEST_DATA_EXTRALIGHT = 3970,
  CMSG_RESPONSE_DATA_EXTRALIGHT = 3986,
  CMSG_LIVESTATUS_EXTENSION = 2083,
  CMSG_SENDNODEID_EXTENSION = 2307,
  CMSG_READNODEID_EXTENSION = 2323,
  CMSG_SENDATEXFLAG_EXTENSION = 2467,
  CMSG_SENDSNR_EXTENSION = 2515,
  CMSG_CONTROL_EXTENSION = 3043,
  CMSG_STATUS_EXTENSION = 3059,
  CMSG_SETCNCMD_EXTENSION = 3174,
  CMSG_GETCNCMD_EXTENSION = 3190,
  CMSG_BLSETCMD_EXTENSION = 3179,
  CMSG_BLGETCMD_EXTENSION = 3195,
  CMSG_SWRESET_EXTENSION = 3169,
  CMSG_SWRESETACK_EXTENSION = 3185,
  CMSG_SENDSTATUS_EXTENSION = 3170,
  CMSG_READSTATUS_EXTENSION = 3186,
  CMSG_EXTENSIONTEMP = 3699,
  CMSG_EXTENSIONPRESSURE = 3715,
  CMSG_EXTENSTIONLIGHTCURRENT = 3731,
  CMSG_REQUEST_DATA_EXTENSION = 3971,
  CMSG_RESPONSE_DATA_EXTENSION = 3987,
  CMSG_LIVESTATUS_CCU1 = 2084,
  CMSG_BUTTONSTATE_FOIL = 2100,
  CMSG_SENDNODEID_CCU1 = 2308,
  CMSG_READNODEID_CCU1 = 2324,
  CMSG_SENDATEXFLAG_CCU1 = 2468,
  CMSG_DCX_CTRL_CCU1 = 2340,
  CMSG_SENDSNR_CCU1 = 2516,
  CMSG_SYSPOWER_CCU1 = 2356,
  CMSG_DCX_RST_CCU1 = 2388,
  CMSG_SATCTRLDATA_CCU1 = 2404,
  CMSG_VIDEOMODE_CCU1 = 2372,
  CMSG_CONTROL_CCU1 = 3044,
  CMSG_STATUS_CCU1 = 3060,
  CMSG_IOCTRLDATA_CCU1 = 2116,
  CMSG_6DPOSITIONDATA_CCU1 = 2132,
  CMSG_SETCNCMD_CCU1 = 3206,
  CMSG_GETCNCMD_CCU1 = 3222,
  CMSG_BLSETCMD_CCU1 = 3211,
  CMSG_BLGETCMD_CCU1 = 3227,
  CMSG_SWRESET_CCU1 = 3201,
  CMSG_SWRESETACK_CCU1 = 3217,
  CMSG_SENDSTATUS_CCU1 = 3202,
  CMSG_READSTATUS_CCU1 = 3218,
  CMSG_REQUEST_DATA_CCU1 = 3972,
  CMSG_RESPONSE_DATA_CCU1 = 3988,
  CMSG_LIVESTATUS_DU = 2085,
  CMSG_SENDNODEID_DU = 2309,
  CMSG_READNODEID_DU = 2325,
  CMSG_SENDATEXFLAG_DU = 2469,
  CMSG_SENDSNR_DU = 2517,
  CMSG_CONTROL_DU = 3045,
  CMSG_STATUS_DU = 3061,
  CMSG_SETCNCMD_DU = 3238,
  CMSG_GETCNCMD_DU = 3254,
  CMSG_BLSETCMD_DU = 3243,
  CMSG_BLGETCMD_DU = 3259,
  CMSG_SWRESET_DU = 3233,
  CMSG_SWRESETACK_DU = 3249,
  CMSG_SENDSTATUS_DU = 3234,
  CMSG_READSTATUS_DU = 3250,
  CMSG_REQUEST_DATA_DU = 3973,
  CMSG_RESPONSE_DATA_DU = 3989,
  CMSG_LIVESTATUS_DCX5000R = 2086,
  CMSG_BUTTONSTATE_DCX5000R = 2102,
  CMSG_JOYSTICKSTATE_DCX5000R = 2118,
  CMSG_INCCNTSTATE_DCX5000R = 2134,
  CMSG_SENDNODEID_DCX5000R = 2310,
  CMSG_READNODEID_DCX5000R = 2326,
  CMSG_SENDATEXFLAG_DCX5000R = 2470,
  CMSG_SENDSNR_DCX5000R = 2518,
  CMSG_CONTROL_DCX5000R = 3046,
  CMSG_STATUS_DCX5000R = 3062,
  CMSG_CTRLPNLMODE_DCX5000R = 2902,
  CMSG_SETCNCMD_DCX5000R = 3270,
  CMSG_GETCNCMD_DCX5000R = 3286,
  CMSG_BLSETCMD_DCX5000R = 3275,
  CMSG_BLGETCMD_DCX5000R = 3291,
  CMSG_SWRESET_DCX5000R = 3265,
  CMSG_SWRESETACK_DCX5000R = 3281,
  CMSG_SENDSTATUS_DCX5000R = 3266,
  CMSG_READSTATUS_DCX5000R = 3282,
  CMSG_REQUEST_DATA_DCX5000R = 3974,
  CMSG_RESPONSE_DATA_DCX5000R = 3990,
  CMSG_LIVESTATUS_DCX5000L = 2087,
  CMSG_BUTTONSTATE_DCX5000L = 2103,
  CMSG_JOYSTICKSTATE_DCX5000L = 2119,
  CMSG_INCCNTSTATE_DCX5000L = 2135,
  CMSG_SENDNODEID_DCX5000L = 2311,
  CMSG_READNODEID_DCX5000L = 2327,
  CMSG_SENDATEXFLAG_DCX5000L = 2471,
  CMSG_SENDSNR_DCX5000L = 2519,
  CMSG_CONTROL_DCX5000L = 3047,
  CMSG_STATUS_DCX5000L = 3063,
  CMSG_CTRLPNLMODE_DCX5000L = 2903,
  CMSG_SETCNCMD_DCX5000L = 3302,
  CMSG_GETCNCMD_DCX5000L = 3318,
  CMSG_BLSETCMD_DCX5000L = 3307,
  CMSG_BLGETCMD_DCX5000L = 3323,
  CMSG_SWRESET_DCX5000L = 3297,
  CMSG_SWRESETACK_DCX5000L = 3313,
  CMSG_SENDSTATUS_DCX5000L = 3298,
  CMSG_READSTATUS_DCX5000L = 3314,
  CMSG_REQUEST_DATA_DCX5000L = 3975,
  CMSG_RESPONSE_DATA_DCX5000L = 3991,
  CMSG_LIVESTATUS_XPECTUC = 2088,
  CMSG_BUTTONSTATE_XPECTUC = 2104,
  CMSG_DATA_XPECTUC = 2120,
  CMSG_SENDNODEID_XPECTUC = 2312,
  CMSG_READNODEID_XPECTUC = 2328,
  CMSG_SENDATEXFLAG_XPECTUC = 2472,
  CMSG_CONTROL_XPECTUC = 3048,
  CMSG_STATUS_XPECTUC = 3064,
  CMSG_SETCNCMD_XPECTUC = 3334,
  CMSG_GETCNCMD_XPECTUC = 3350,
  CMSG_BLSETCMD_XPECTUC = 3339,
  CMSG_BLGETCMD_XPECTUC = 3355,
  CMSG_SWRESET_XPECTUC = 3329,
  CMSG_SWRESETACK_XPECTUC = 3345,
  CMSG_SENDSTATUS_XPECTUC = 3330,
  CMSG_READSTATUS_XPECTUC = 3346,
  CMSG_XPECTPRESSURE = 3720,
  CMSG_REQUEST_DATA_XPECTUC = 3976,
  CMSG_RESPONSE_DATA_XPECTUC = 3992,
  CMSG_LIVESTATUS_XPECTIMX = 2089,
  CMSG_DATA_XPECTIMX = 2121,
  CMSG_SENDNODEID_XPECTIMX = 2313,
  CMSG_READNODEID_XPECTIMX = 2329,
  CMSG_SENDATEXFLAG_XPECTIMX = 2473,
  CMSG_SETMETERCNT1VALUE_XPECTIMX = 2601,
  CMSG_CONTROL_XPECTIMX = 3049,
  CMSG_STATUS_XPECTIMX = 3065,
  CMSG_CTRLPNLMODE_XPECTIMX = 2905,
  CMSG_SETCNCMD_XPECTIMX = 3366,
  CMSG_GETCNCMD_XPECTIMX = 3382,
  CMSG_BLSETCMD_XPECTIMX = 3371,
  CMSG_BLGETCMD_XPECTIMX = 3387,
  CMSG_SWRESET_XPECTIMX = 3361,
  CMSG_SWRESETACK_XPECTIMX = 3377,
  CMSG_SENDSTATUS_XPECTIMX = 3362,
  CMSG_READSTATUS_XPECTIMX = 3378,
  CMSG_REQUEST_DATA_XPECTIMX = 3977,
  CMSG_RESPONSE_DATA_XPECTIMX = 3993,
  CMSG_LIVESTATUS_WID = 2091,
  CMSG_BUTTONSTATE_WID = 2107,
  CMSG_DATA_WID = 2123,
  CMSG_SENDNODEID_WID = 2315,
  CMSG_READNODEID_WID = 2331,
  CMSG_SENDATEXFLAG_WID = 2475,
  CMSG_CONTROL_WID = 3051,
  CMSG_STATUS_WID = 3067,
  CMSG_SETCNCMD_WID = 3430,
  CMSG_GETCNCMD_WID = 3446,
  CMSG_BLSETCMD_WID = 3435,
  CMSG_BLGETCMD_WID = 3451,
  CMSG_SWRESET_WID = 3425,
  CMSG_SWRESETACK_WID = 3441,
  CMSG_SENDSTATUS_WID = 3426,
  CMSG_READSTATUS_WID = 3442,
  CMSG_REQUEST_DATA_WID = 3979,
  CMSG_RESPONSE_DATA_WID = 3995,
  CMSG_LIVESTATUS_WIM = 2090,
  CMSG_DATA_WIM = 2122,
  CMSG_SENDNODEID_WIM = 2314,
  CMSG_READNODEID_WIM = 2330,
  CMSG_SENDATEXFLAG_WIM = 2474,
  CMSG_CONTROL_WIM = 3050,
  CMSG_STATUS_WIM = 3066,
  CMSG_CTRLPNLMODE_WIM = 2906,
  CMSG_SETCNCMD_WIM = 3398,
  CMSG_GETCNCMD_WIM = 3414,
  CMSG_BLSETCMD_WIM = 3403,
  CMSG_BLGETCMD_WIM = 3419,
  CMSG_SWRESET_WIM = 3393,
  CMSG_SWRESETACK_WIM = 3409,
  CMSG_SENDSTATUS_WIM = 3394,
  CMSG_READSTATUS_WIM = 3410,
  CMSG_REQUEST_DATA_WIM = 3978,
  CMSG_RESPONSE_DATA_WIM = 3994,
  CMSG_LIVESTATUS_BYD = 2092,
  CMSG_DATA_BYD = 2124,
  CMSG_METERCNT1_BYD = 2396,
  CMSG_SENDNODEID_BYD = 2316,
  CMSG_READNODEID_BYD = 2332,
  CMSG_SENDATEXFLAG_BYD = 2476,
  CMSG_CONTROL_BYD = 3052,
  CMSG_STATUS_BYD = 3068,
  CMSG_SETCNCMD_BYD = 3462,
  CMSG_GETCNCMD_BYD = 3478,
  CMSG_BLSETCMD_BYD = 3467,
  CMSG_BLGETCMD_BYD = 3483,
  CMSG_SWRESET_BYD = 3457,
  CMSG_SWRESETACK_BYD = 3473,
  CMSG_SENDSTATUS_BYD = 3458,
  CMSG_READSTATUS_BYD = 3474,
  CMSG_REQUEST_DATA_BYD = 3980,
  CMSG_RESPONSE_DATA_BYD = 3996,
  CMSG_LIVESTATUS_AR = 2093,
  CMSG_DATA_AR = 2125,
  CMSG_METERCNT1_AR = 2397,
  CMSG_AKKUSTATUS_AR = 2413,
  CMSG_POWERSTATUS_AR = 2429,
  CMSG_SENDNODEID_AR = 2317,
  CMSG_READNODEID_AR = 2333,
  CMSG_SENDATEXFLAG_AR = 2477,
  CMSG_CONTROL_AR = 3053,
  CMSG_STATUS_AR = 3069,
  CMSG_SETCNCMD_AR = 3494,
  CMSG_GETCNCMD_AR = 3510,
  CMSG_BLSETCMD_AR = 3499,
  CMSG_BLGETCMD_AR = 3515,
  CMSG_SWRESET_AR = 3489,
  CMSG_SWRESETACK_AR = 3505,
  CMSG_SENDSTATUS_AR = 3490,
  CMSG_READSTATUS_AR = 3506,
  CMSG_REQUEST_DATA_AR = 3981,
  CMSG_RESPONSE_DATA_AR = 3997,
  CMSG_LIVESTATUS_RV2 = 2094,
  CMSG_DATA_RV2 = 2126,
  CMSG_SENDNODEID_RV2 = 2318,
  CMSG_READNODEID_RV2 = 2334,
  CMSG_SENDATEXFLAG_RV2 = 2478,
  CMSG_BRR_STATUS_RV2 = 2974,
  CMSG_CONTROL_RV2 = 3054,
  CMSG_STATUS_RV2 = 3070,
  CMSG_SETCNCMD_RV2 = 3526,
  CMSG_GETCNCMD_RV2 = 3542,
  CMSG_BLSETCMD_RV2 = 3531,
  CMSG_BLGETCMD_RV2 = 3547,
  CMSG_SWRESET_RV2 = 3521,
  CMSG_SWRESETACK_RV2 = 3537,
  CMSG_SENDSTATUS_RV2 = 3522,
  CMSG_READSTATUS_RV2 = 3538,
  CMSG_REQUEST_DATA_RV2 = 3982,
  CMSG_RESPONSE_DATA_RV2 = 3998

} MessageId;

typedef signed char CHAR;
typedef signed char SCHAR;
typedef unsigned char UCHAR;
typedef unsigned char USHORT;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef struct
{
  uint32_t StdId; /*!< Specifies the standard identifier.
                       This parameter can be a value between 0 to 0x7FF. */

  uint32_t ExtId; /*!< Specifies the extended identifier.
                       This parameter can be a value between 0 to 0x1FFFFFFF. */

  uint8_t IDE; /*!< Specifies the type of identifier for the message that
                    will be received. This parameter can be a value of
                    @ref CAN_identifier_type */

  uint8_t RTR; /*!< Specifies the type of frame for the received message.
                    This parameter can be a value of
                    @ref CAN_remote_transmission_request */

  uint8_t DLC; /*!< Specifies the length of the frame that will be received.
                    This parameter can be a value between 0 to 8 */

  uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to
                        0xFF. */

  uint8_t FMI; /*!< Specifies the index of the filter the message stored in
                    the mailbox passes through. This parameter can be a
                    value between 0 to 0xFF */
} CanRxMsg;

typedef struct
{
  uint32_t StdId; /*!< Specifies the standard identifier.
                       This parameter can be a value between 0 to 0x7FF. */

  uint32_t ExtId; /*!< Specifies the extended identifier.
                       This parameter can be a value between 0 to 0x1FFFFFFF. */

  uint8_t IDE; /*!< Specifies the type of identifier for the message that
                    will be transmitted. This parameter can be a value
                    of @ref CAN_identifier_type */

  uint8_t RTR; /*!< Specifies the type of frame for the message that will
                    be transmitted. This parameter can be a value of
                    @ref CAN_remote_transmission_request */

  uint8_t DLC; /*!< Specifies the length of the frame that will be
                    transmitted. This parameter can be a value between
                    0 to 8 */

  uint8_t Data[8]; /*!< Contains the data to be transmitted. It ranges from 0
                        to 0xFF. */
} CanTxMsg;

typedef struct
{

  uint8_t c[8];

} data;

typedef struct
{
  uint32_t MsgID;

  uint8_t DataLength;

  data Data;

} CantxMsg;

typedef union
{
  uint8_t data[4];
  uint32_t id;
} CAN_ID;

typedef struct
{
  CAN_ID StdId;

  CAN_ID ExtId;

} CanIDMsg;
/* Legacy defines */
#define CAN_RTR_Data ((uint32_t)0x00000000)   /*!< Data frame */
#define CAN_RTR_Remote ((uint32_t)0x00000002) /*!< Remote frame */
#define CAN_RTR_DATA CAN_RTR_Data
#define CAN_RTR_REMOTE CAN_RTR_Remote
#define CAN_Id_Standard ((uint32_t)0x00000000) /*!< Standard Id */
#define CAN_Id_Extended ((uint32_t)0x00000004) /*!< Extended Id */
#define CAN_ID_STD CAN_Id_Standard
#define CAN_ID_EXT CAN_Id_Extended
extern CanRxMsg RxMessage;
extern CanIDMsg CanToWiFiID;
extern CanIDMsg WiFiToCanID;
extern MessageId CANMessageId;
extern CantxMsg CanTMsg;
extern uint16_t CAN_Cnt;
extern void SendPowerModeNormal(void);
extern void SendExtraLightEvent(SCHAR scV1);
extern void SendCrawlerSpeedValue(SCHAR scV1, SCHAR scV2, UCHAR ucMode);
extern void SendLaserPointEvent(SCHAR scV1);
extern void SetLocatorConfigEvent(SCHAR uVal, SCHAR uVa2);
extern void SendStartupCMD(void);
extern void SendPowerOffCMD(void);
extern void SendMainLightEvent(SCHAR scV1);
extern void SendautoAngleMainLightsStatus(UCHAR ucMode);
extern void SendautoAngleMainLightsValueInDegrees(SCHAR scV1);
extern void SendReelPower(SCHAR scValue);
extern void SendReelSpeed(SCHAR scValue);
extern void SendReelFunctionCodeEvent(SCHAR scMode);
extern void SendCameraJoystickEvent(SCHAR scV1, SCHAR scV2);
extern void SendCameraHomePositionEvent(void);
extern void SendCameraZoomEvent(SCHAR scV);
extern void SendCrawlerSpeedValue(SCHAR scV1, SCHAR scV2, UCHAR ucMode);
extern void SendFullStop(void);
#endif
