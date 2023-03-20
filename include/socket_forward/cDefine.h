/*
 * @Author       : iPEK
 * @Date         : 2023-03-02
 * @LastEditTime: 2023-03-12 20:28:36
 * @Description  : TCP�DP餈靽⊥��喳�銋?
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __C_DEFINED_H
#define __C_DEFINED_H
#include <stdint.h>

#define MY_TCP_PORT 10086   // �?�配cp蝡?��
#define DEST_TCP_PORT 8095  // �?�cp蝡?��
#define MY_UDP_PORT 10010   // �?�酌dp蝡?��
#define DEST_UDP_PORT 10000 // �?�dp蝡?��

#define MY_HOST "192.168.16.254"     // �?�迺p
#define DEST_UDP_IP "192.168.16.10"  // udp�?�p
#define DEST_TCP_IP "192.168.16.100" // tcp�?�p

#define UDP_SEND_FREQUENCY 100; // udp��憭折?�(Hz)
#define TCP_SEND_FREQUENCY 500; // tcp��憭折?�(Hz)


// #define MY_HOST "192.168.1.146"     // �砍ip
// #define DEST_UDP_IP "192.168.1.145" // udp�格�ip
// #define DEST_TCP_IP "192.168.1.145" // tcp�格�ip


#define DEBUG 1

#endif