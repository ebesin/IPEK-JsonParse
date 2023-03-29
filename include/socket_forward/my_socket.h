/*
 * @Author       : iPEK
 * @Date         : 2023-03-02
 * @LastEditTime : 2023-03-10
 * @Description  :
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __MY_SOCKET_H
#define __MY_SOCKET_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/select.h>
#include <string.h>
#include "cDefine.h"
#include "../common/cJSON.h"

/**
 * @description  : tcp信息结构体，该版本openwrt有关该结构体的头文件缺失，这里重新定义了。
 *                 可用于判断tcp连接状态，在连接断开时进行重连等操作。
 */
struct tcp_info
{
    uint8_t tcpi_state;
    uint8_t tcpi_ca_state;
    uint8_t tcpi_retransmits;
    uint8_t tcpi_probes;
    uint8_t tcpi_backoff;
    uint8_t tcpi_options;
    uint8_t tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4;

    uint32_t tcpi_rto;
    uint32_t tcpi_ato;
    uint32_t tcpi_snd_mss;
    uint32_t tcpi_rcv_mss;

    uint32_t tcpi_unacked;
    uint32_t tcpi_sacked;
    uint32_t tcpi_lost;
    uint32_t tcpi_retrans;
    uint32_t tcpi_fackets;

    /* Times. */
    uint32_t tcpi_last_data_sent;
    uint32_t tcpi_last_ack_sent;
    uint32_t tcpi_last_data_recv;
    uint32_t tcpi_last_ack_recv;

    /* Metrics. */
    uint32_t tcpi_pmtu;
    uint32_t tcpi_rcv_ssthresh;
    uint32_t tcpi_rtt;
    uint32_t tcpi_rttvar;
    uint32_t tcpi_snd_ssthresh;
    uint32_t tcpi_snd_cwnd;
    uint32_t tcpi_advmss;
    uint32_t tcpi_reordering;

    uint32_t tcpi_rcv_rtt;
    uint32_t tcpi_rcv_space;

    uint32_t tcpi_total_retrans;
};

typedef struct StartUpProcessCmd
{
    char cmd[50];     // 指令
    double send_time; // 发送时间
} StartUpProcessCmd;

/**
 * @description  : 初始化udp套接字，包括句柄，本地套接字，目标套接字
 * @param         {int} *udpSock:                       本地TCP操作句柄指针
 * @param         {sockaddr_in} *my_udp_sock_addr:      本地UDP套接字
 * @param         {sockaddr_in} *dest_udp_sock_addr:    目标UDP套接字
 * @return        {*}
 */
void initUDP(int *udpSock, struct sockaddr_in *my_udp_sock_addr, struct sockaddr_in *dest_udp_sock_addr);

/**
 * @description  : 初始化tcp套接字，包括句柄，本地套接字，目标套接字
 * @param         {int} *tcpSock:                       本地TCP操作句柄指针
 * @param         {sockaddr_in} *my_tcp_addr:           本地TCP套接字
 * @param         {sockaddr_in} *dest_tcp_addr:         目标TCP套接字
 * @return        {*}
 */
void initTCP(int *tcpSock, struct sockaddr_in *my_tcp_addr, struct sockaddr_in *dest_tcp_addr);

/**
 * @description  : 初始化socket，包括进行tcp、udp等的初始化及tcp的连接
 * @return        {*}
 */
void initSocket();

/**
 * @description  : 连接到TCP服务器，直到连接成功
 * @return        {*}
 */
void connectToHost();

/**
 * @description  :  通过udp发送指令到管道机器人(不是真正的发送，只是将数据入队)
 * @param         {char} send_buf:      发送的buf
 * @param         {int} len:            发送的数据长度
 * @return        {*}
 */
void sendToRobot(char send_buf[], int len);

/**
 * @description  :  通过tcp发送json字串到上位机(不是真正的发送，只是将数据入队)
 * @param         {char} *send_str:     发送的字串
 * @return        {*}
 */
void sendToApp(char *send_str);

// 监听tcp和udp并转发
/**
 * @description  :  该函数主要负责监听udp及tcp端口数据，并在接收到数据后交由各自解析器进行解析，解析完成后进行数据的转发
 * @return        {*}
 */
void listenAndForward();

/**
 * @description  : udp发送线程
 * @return        {*}
 */
void *udp_send_thread(void *arg);

/**
 * @description  : tcp发送线程
 * @return        {*}
 */
void *tcp_send_thread(void *arg);

/**
 * @description  : 获取当前系统时间（微秒级），主要用于调试，计算程序运行速度
 * @return        {*}
 */
long int getCurrentTime();

#endif