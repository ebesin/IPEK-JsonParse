/*
 * @Author       : iPEK
 * @Date         : 2023-03-02
 * @LastEditTime: 2023-03-12 20:28:36
 * @Description  : TCP和UDP连接信息的相关定�??
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __C_DEFINED_H
#define __C_DEFINED_H
#include <stdint.h>

#define MY_TCP_PORT 10086   // �??地tcp�??口号
#define DEST_TCP_PORT 8095  // �??标tcp�??口号
#define MY_UDP_PORT 10010   // �??地udp�??口号
#define DEST_UDP_PORT 10000 // �??标udp�??口号

#define MY_HOST "192.168.16.254"     // �??地ip
#define DEST_UDP_IP "192.168.16.10"  // udp�??标ip
#define DEST_TCP_IP "192.168.16.100" // tcp�??标ip

#define UDP_SEND_FREQUENCY 100; // udp发送最大�?�率(Hz)
#define TCP_SEND_FREQUENCY 500; // tcp发送最大�?�率(Hz)

// #define MY_HOST "192.168.1.146"     // �??地ip
// #define DEST_UDP_IP "192.168.1.176" // udp�??标ip
// #define DEST_TCP_IP "192.168.1.176" // tcp�??标ip

#define DEBUG 0

#endif