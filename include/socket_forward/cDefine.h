/*
 * @Author       : iPEK
 * @Date         : 2023-03-02
 * @LastEditTime: 2023-03-12 20:28:36

 * @Description  : TCPå’ŒUDPè¿æ¥ä¿¡æ¯çš„ç›¸å…³å®šä¹?

 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __C_DEFINED_H
#define __C_DEFINED_H
#include <stdint.h>

#define MY_TCP_PORT 10086   // æœ?åœ°tcpç«?å£å·
#define DEST_TCP_PORT 8095  // ç›?æ ‡tcpç«?å£å·
#define MY_UDP_PORT 10010   // æœ?åœ°udpç«?å£å·
#define DEST_UDP_PORT 10000 // ç›?æ ‡udpç«?å£å·

#define MY_HOST "192.168.16.254"     // æœ?åœ°ip
#define DEST_UDP_IP "192.168.16.10"  // udpç›?æ ‡ip
#define DEST_TCP_IP "192.168.16.100" // tcpç›?æ ‡ip

#define UDP_SEND_FREQUENCY 100; // udpå‘é€æœ€å¤§é?‘ç‡(Hz)
#define TCP_SEND_FREQUENCY 500; // tcpå‘é€æœ€å¤§é?‘ç‡(Hz)

// #define MY_HOST "192.168.1.146"     // æœ?åœ°ip
// #define DEST_UDP_IP "192.168.1.145" // udpç›?æ ‡ip
// #define DEST_TCP_IP "192.168.1.145" // tcpç›?æ ‡ip

#define DEBUG 0

#endif