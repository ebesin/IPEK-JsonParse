/*
 * @Author       : iPEK
 * @Date         : 2023-03-02
 * @LastEditTime: 2023-03-12 20:28:36

 * @Description  : TCP和UDP连接信息的相关定义

 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __C_DEFINED_H
#define __C_DEFINED_H
#include <stdint.h>

#define MY_TCP_PORT 10086   // 本地tcp端口号
#define DEST_TCP_PORT 8095  // 目标tcp端口号
#define MY_UDP_PORT 10010   // 本地udp端口号
#define DEST_UDP_PORT 10000 // 目标udp端口号

#define MY_HOST "192.168.16.254"     // 本地ip
#define DEST_UDP_IP "192.168.16.10"  // udp目标ip
#define DEST_TCP_IP "192.168.16.100" // tcp目标ip

#define UDP_SEND_FREQUENCY 100; // udp发送最大频率(Hz)
#define TCP_SEND_FREQUENCY 500; // tcp发送最大频率(Hz)

// #define MY_HOST "192.168.1.146"     // 本地ip
// #define DEST_UDP_IP "192.168.1.145" // udp目标ip
// #define DEST_TCP_IP "192.168.1.145" // tcp目标ip

#define DEBUG 0

#endif