/*
 * @Author       : iPEK
 * @Date         : 2023-03-02
 * @LastEditTime: 2023-03-09 13:39:01
 * @Description  : 主要用于建立与上位机的TCP连接以及与机器人端的UDP连接，主要功能有：
 *                 1. 将上位机通过TCP发送的Json指令进行指令转换(json->can)后通过UDP发送给管道机器人；
 *                 2. 将管道机器人通过UDP发送的CAN指令进行指令转换(can->json)后通过TCP发送给上位机。
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#include "my_socket.h"
#include "circle_queue.h"
#include <pthread.h>
#include "../json_code/task.h"

// udp句柄
int my_udp_sock = -1;
// tcp句柄
int my_tcp_sock = -1;
// 本地udp套接字
struct sockaddr_in my_udp_sock_addr;
// 目标udp套接字
struct sockaddr_in dest_udp_sock_addr;
// 本地tcp套接字
struct sockaddr_in my_tcp_sock_addr;
// 目标udp套接字
struct sockaddr_in dest_tcp_sock_addr;
// tcp信息
struct tcp_info info;
int finished_start_up = 0;
int finished_video_streaming = 0;
// tcp发送队列
CircleQueue tcp_send_queue;
// udp发送队列
CircleQueue udp_send_queue;
// udp线程标识符
pthread_t t_udp;
// tcp线程标识符
pthread_t t_tcp;
// 开机过程线程标识符
pthread_t t_start_up;
// 开启视频流发送线程标识符
pthread_t t_start_video_streaming;
// udp互斥锁
pthread_mutex_t udp_mutex = PTHREAD_MUTEX_INITIALIZER;
// tcp互斥锁
pthread_mutex_t tcp_mutex = PTHREAD_MUTEX_INITIALIZER;

void initUDP(int *udp_sock, struct sockaddr_in *my_udp_sock_addr, struct sockaddr_in *dest_udp_sock_addr)
{
    if ((*udp_sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        exit(1);
    int opt = 1;
    setsockopt(*udp_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 绑定本地udp套接字
    my_udp_sock_addr->sin_family = AF_INET;
    my_udp_sock_addr->sin_port = htons(MY_UDP_PORT);
    unsigned int iRemoteAddr = 0;
    inet_pton(AF_INET, MY_HOST, &iRemoteAddr);
    my_udp_sock_addr->sin_addr.s_addr = iRemoteAddr;
    if (bind(*udp_sock, (struct sockaddr *)my_udp_sock_addr, sizeof(*my_udp_sock_addr)) < 0)
    {
        perror("Error:bindudp");
        exit(1);
    }
    // 绑定目标udp套接字
    socklen_t len = sizeof(*dest_udp_sock_addr);
    dest_udp_sock_addr->sin_family = AF_INET;
    dest_udp_sock_addr->sin_addr.s_addr = inet_addr(DEST_UDP_IP);
    dest_udp_sock_addr->sin_port = htons(DEST_UDP_PORT);
}

void initTCP(int *tcp_sock, struct sockaddr_in *my_tcp_addr, struct sockaddr_in *dest_tcp_addr)
{
    close(*tcp_sock);
    /* 创建socket */
    if ((*tcp_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error:socket");
        exit(1);
    }
    int opt = 1;
    setsockopt(*tcp_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定本地tcp套接字
    bzero(my_tcp_addr, sizeof(*my_tcp_addr));
    unsigned int myIRemoteAddr = 0;
    my_tcp_addr->sin_family = AF_INET;
    my_tcp_addr->sin_port = htons(MY_TCP_PORT);
    inet_pton(AF_INET, MY_HOST, &myIRemoteAddr);
    my_tcp_addr->sin_addr.s_addr = myIRemoteAddr;
    if (bind(*tcp_sock, (struct sockaddr *)my_tcp_addr, sizeof(*my_tcp_addr)) < 0)
    {
        perror("Error:bindtcp");
        exit(1);
    }

    // 绑定目标tcp套接字
    bzero(dest_tcp_addr, sizeof(*dest_tcp_addr));
    unsigned int destIRemoteAddr = 0;
    dest_tcp_addr->sin_family = AF_INET;
    dest_tcp_addr->sin_port = htons(DEST_TCP_PORT);
    inet_pton(AF_INET, DEST_TCP_IP, &destIRemoteAddr);
    dest_tcp_addr->sin_addr.s_addr = destIRemoteAddr;
}

long int getCurrentTime()
{
    struct timeval tp;
    gettimeofday(&tp, 0);

    return tp.tv_usec;
}

void sendToRobot(char send_buf[], int len)
{
    SocketData socket_data;
    socket_data.len = len;
    for (int i = 0; i < len; i++)
        socket_data.data[i] = send_buf[i];
    pthread_mutex_lock(&udp_mutex);
    enQueue(&udp_send_queue, &socket_data);
    pthread_mutex_unlock(&udp_mutex);
}

void sendToApp(char *send_str)
{
    int len = strlen(send_str);
    SocketData socket_data;
    socket_data.len = len;
    for (int i = 0; i < len; i++)
        socket_data.data[i] = send_str[i];
    pthread_mutex_lock(&tcp_mutex);
    enQueue(&tcp_send_queue, &socket_data);
    pthread_mutex_unlock(&tcp_mutex);
}

void onVideoStreamConfirm()
{
    finished_video_streaming = 1;
}

void *udp_send_thread(void *args)
{
    int interval = 1000 * 1000 / UDP_SEND_FREQUENCY;

    while (1)
    {
        if (!queueEmpty(&udp_send_queue))
        {
            SocketData send_data;
            pthread_mutex_lock(&udp_mutex);
            deQueue(&udp_send_queue, &send_data);
            pthread_mutex_unlock(&udp_mutex);
            sendto(my_udp_sock, send_data.data, send_data.len, 0, (struct sockaddr *)&dest_udp_sock_addr, sizeof(dest_udp_sock_addr));
        }
        usleep(interval);
    }
}

void *tcp_send_thread(void *args)
{

    int interval = 1000 * 1000 / UDP_SEND_FREQUENCY;
    printf("interval:%d\n", interval);
    while (1)
    {
        if (!queueEmpty(&tcp_send_queue))
        {
            SocketData send_data;
            pthread_mutex_lock(&tcp_mutex);
            deQueue(&tcp_send_queue, &send_data);
            pthread_mutex_unlock(&tcp_mutex);
            send(my_tcp_sock, send_data.data, send_data.len, 0);
        }
        usleep(interval);
    }
}

void *start_up_process(void *args)
{
    // 初始化指令
    char *initialization_cmd = "{\"header\":{\"messageType\":\"IPEK_CHINA_GUI\",\"messageName\":\"INITIALIZATION\"},\"payload\":{\"highBeamMainLightsValueInPercent\":0,\"lowBeamMainLightsValueInPercent\":0,\"autoAngleMainLightsStatus\":true,\"autoAngleMainLightsValueInDegrees\":0,\"auxiliaryLightsValueInPercent\":0,\"laserIntensity\":\"0%\",\"zoomingStatus\":false,\"focusType\":\"auto\",\"focusingStatus\":false,\"elevatorStatus\":false,\"cameraChosen\":\"front\",\"rearCameraAmount\":2,\"rearCameraIdx\":1,\"temperatureInCelsius\":25,\"pressureInHpa\":500,\"pressureStatus\":\"ok\",\"crawlerAngleInDegrees\":-2,\"crawlerOver\":\"ok\",\"localizerFrequency\":\"512Hz\",\"clutchStatus\":true,\"cruiseControlStatus\":false,\"cruiseControlValue\":0,\"cableReelType\":\"automatic\",\"cableReelPower\":10,\"cableReelSpeed\":0,\"elevatorValueInPercent\":0,\"elevatorValueInMm\":0}}\n";
    StartUpProcessCmd start_up_process_cmds[] = {
        {{0xF4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}, 0},              // 开机
        {{0x34, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01}, 5},        // 开电
        {{0xE4, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x13, 0x01}, 17}, // 急停
        {{0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00}, 17.5},     // 离合器
        {{0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}, 17.5},           // 离合器
        {{0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}, 18},             // 激光关
        {{0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}, 18},             // 激光关
        {{0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}, 18.5},           // 辅助光源关
        {{0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}, 19},             // 相机灯光关
        {{0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x37}, 24},             // 自动对焦
        {{0x34, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}, 24.5},           // 绕线盘自动模式
        {{0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0A}, 25},             // 绕线盘功率10
        {{0xE4, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x13, 0x01}, 36}, // 急停
        {{0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x89}, 36.5},           // 第一条对焦
        {{0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xD3}, 37},             // 对焦
        {{0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x37}, 37},             // 对焦
    };
    sendToApp(initialization_cmd);
    int cmd_len = sizeof(start_up_process_cmds) / sizeof(StartUpProcessCmd);
    printf("cmd_len:\t%d\n", cmd_len);
    for (int i = 0; i < cmd_len - 1; i++)
    {
        sendToRobot(start_up_process_cmds[i].cmd, start_up_process_cmds[i].cmd[10] + 11);
        usleep((start_up_process_cmds[i + 1].send_time - start_up_process_cmds[i].send_time) * 1000000);
    }
    sendToRobot(start_up_process_cmds[cmd_len - 1].cmd, start_up_process_cmds[cmd_len - 1].cmd[10] + 11);
}

void *start_video_streaming(void *args)
{
    char *start_video_cmd = "{\"header\":{\"messageId\":102,\"messageName\":\"START_VIDEO_STREAMING_REQ\",\"messageType\":\"VIDEO\"},\"payload\":{\"port\":5000}}\n";
    while (!finished_video_streaming)
    {
        sendToApp(start_video_cmd);
        sleep(1);
    }
}

void connectToHost()
{
reconnect:
    initTCP(&my_tcp_sock, &my_tcp_sock_addr, &dest_tcp_sock_addr);
    // 若连接失败则重连
    if (connect(my_tcp_sock, (void *)&dest_tcp_sock_addr, sizeof(dest_tcp_sock_addr)) < 0)
    {
        perror("Error:connect");
        close(my_tcp_sock);
        sleep(1);
        goto reconnect;
    }
    else
    {
        if (!finished_start_up)
        {
            pthread_create(&t_start_up, NULL, start_up_process, NULL);
            pthread_create(&t_start_video_streaming, NULL, start_video_streaming, NULL);
            finished_start_up = 1;
        }
    }
}

void initSocket()
{
    initUDP(&my_udp_sock, &my_udp_sock_addr, &dest_udp_sock_addr);
    connectToHost();
    initQueue(&tcp_send_queue);
    initQueue(&udp_send_queue);
}

void listenAndForward()
{
    fd_set rset;
    FD_ZERO(&rset);
    char recv_buf[1024] = {0};
    char buffer[2048] = {0};
    int udp_num = 0;
    int maxfd = 0;
    char temp[1024] = {0};
    cJSON *str_json;

    pthread_create(&t_tcp, NULL, tcp_send_thread, NULL);
    pthread_create(&t_udp, NULL, udp_send_thread, NULL);
    while (1)
    {
        int len = sizeof(info);
        getsockopt(my_tcp_sock, IPPROTO_TCP, 11, &info, (socklen_t *)&len);
        if (info.tcpi_state != 1)
        {
#if DEBUG
            printf("socket Reconnecting....\n");
#endif
            close(my_tcp_sock);
            sleep(1);
            connectToHost();
        }
        FD_ZERO(&rset);
        FD_SET(my_udp_sock, &rset);
        FD_SET(my_tcp_sock, &rset);
        maxfd = my_udp_sock > my_tcp_sock ? my_udp_sock : my_tcp_sock;
        switch (select(maxfd + 1, &rset, NULL, NULL, NULL))
        {
        case -1:
#if DEBUG
            printf("error");
#endif
            break;
        case 0:
            break;
        default:
            if (FD_ISSET(my_udp_sock, &rset))
            {
                long int before = getCurrentTime();
                struct sockaddr_in peeraddr;
                socklen_t peerlen = sizeof(peeraddr);
                memset(recv_buf, 0, sizeof(recv_buf));
                int n = recvfrom(my_udp_sock, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&peeraddr, &peerlen);
#if DEBUG
                printf("udp len:%d\n", n);
#endif
                if (n > 0)
                {
                    udp_num++;
#if DEBUG
                    printf("udp receive=========>%d\n", udp_num);
#endif
                    Scheduler_Code(recv_buf);
                }
                long int after = getCurrentTime();
#if DEBUG
                printf("time:%ld\n", after - before);
#endif
            }
            if (FD_ISSET(my_tcp_sock, &rset))
            {
                memset(recv_buf, 0, sizeof(recv_buf));
                int n = recv(my_tcp_sock, recv_buf, sizeof(recv_buf), 0);
#if DEBUG
                printf("tcp len:%d\n", n);
#endif
                if (n > 0)
                {
                    // 把接收到的拼接到buffer末尾
                    strncat(buffer, recv_buf, sizeof(recv_buf));
#if DEBUG
                    printf("buffer_len:%ld\n", strlen(buffer));
                    printf("buffer:%s\n", buffer);
#endif
                    long int before = getCurrentTime();
                    int offset = 0;
                    str_json = cJSON_ParseWithOptsLen(buffer, 0, &offset);
                    while (str_json)
                    {
// 解析成功，parse_end里面是剩下的字符，把buffer清空，把parse_end拼接到buffer内,直到解析不出json
#if DEBUG
                        printf("success\n");
                        printf("offest:%d\n", offset);
#endif
                        Scheduler_Decode(str_json);
                        memset(temp, 0, sizeof(temp));
                        if (offset > 0)
                        {
                            for (int i = 0;; i++)
                            {
                                temp[i] = buffer[offset + i];
                                if (temp[i] == '\0')
                                    break;
                            }
                        }
                        memset(buffer, 0, strlen(buffer));
                        strncat(buffer, temp, sizeof(temp));
#if DEBUG
                        printf("buffer:%s\n", buffer);
#endif
                        cJSON_Delete(str_json);
                        str_json = NULL;
                        str_json = cJSON_ParseWithOptsLen(buffer, 0, &offset);
                    }
                    cJSON_Delete(str_json);
                    // 如果解析不出json，要么是buffer为空，要么是没有合法json字串
                    // todo:加一个长度限制机制，若大于800字节仍未有效json字串，则清空
                    if (strlen(buffer) > 800)
                        memset(buffer, 0, strlen(buffer));
                    long int after = getCurrentTime();
#if DEBUG
                    printf("time:%ld\n", after - before);
#endif
                }
            }
        }
    }
}