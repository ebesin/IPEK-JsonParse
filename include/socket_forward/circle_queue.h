/*
 * @Author       : iPEK
 * @Date         : 2023-03-10
 * @LastEditTime : 2023-03-10
 * @Description  : 循环队列
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */
#ifndef __CIRCIE_QUEUE_H
#define __CIRCIE_QUEUE_H

#define MAXSIZE 50

typedef struct SocketData
{
    char data[1000];
    int len;
} SocketData;

/**
 * @description  : 循环队列基本结构
 * @return        {*}
 */
typedef struct CircleQueue
{
    /* data */
    int front; // 指向头
    int rear;  // 指向尾
    SocketData buffer[MAXSIZE];
} CircleQueue;

/**
 * @description  : 初始化队列
 * @param         {CircleQueue} *:
 * @return        {*}
 */
void initQueue(CircleQueue *);

/**
 * @description  : 判断队列是否为空
 * @param         {CircleQueue} *:
 * @return        {*}
 */
int queueEmpty(const CircleQueue *);

/**
 * @description  : 获取队列长度
 * @param         {CircleQueue} *:
 * @return        {*}
 */
int queueLength(const CircleQueue *);

/**
 * @description  : 入队操作
 * @param         {CircleQueue} *:  要操作的队列
 * @param         {SocketData} *:   入队的元素
 * @return        {*}
 */
void enQueue(CircleQueue *, const SocketData *);

/**
 * @description  : 出队操作
 * @param         {CircleQueue} *:  要操作的队列
 * @param         {SocketData} *:   出队的元素
 * @return        {*}
 */
void deQueue(CircleQueue *, SocketData *);

/**
 * @description  : 清空队列（只是逻辑上的清空）
 * @param         {CircleQueue} *:
 * @return        {*}
 */
void clearQueue(CircleQueue *);

#endif