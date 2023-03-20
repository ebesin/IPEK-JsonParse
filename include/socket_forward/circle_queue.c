/*
 * @Author       : iPEK
 * @Date         : 2023-03-10
 * @LastEditTime : 2023-03-10
 * @Description  : 循环队列操作的实现
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */

#include "circle_queue.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void initQueue(CircleQueue *cq)
{
    cq->front = -1;
    cq->rear = -1;
}

int queueEmpty(const CircleQueue *cq)
{
    return cq->front == cq->rear;
}

int queueLength(const CircleQueue *cq)
{
    return (cq->rear - cq->front + MAXSIZE) % MAXSIZE;
}

void enQueue(CircleQueue *cq, const SocketData *e)
{
    if ((cq->rear + 1) % MAXSIZE == cq->front)
    {
        // 若队满，第一个元素出队
        SocketData tmp;
        deQueue(cq, &tmp);
    }
    cq->rear = (cq->rear + 1) % MAXSIZE;
    cq->buffer[cq->rear].len = e->len;
    for (int i = 0; i < e->len; i++)
    {
        cq->buffer[cq->rear]
            .data[i] = e->data[i];
    }
}

void deQueue(CircleQueue *cq, SocketData *e)
{
    if (queueEmpty(cq))
        return;
    cq->front = (cq->front + 1) % MAXSIZE;
    e->len = cq->buffer[cq->front].len;
    for (int i = 0; i < cq->buffer[cq->front].len; i++)
        e->data[i] = cq->buffer[cq->front].data[i];
}

void clearQueue(CircleQueue *cq)
{
    while (!queueEmpty(cq))
    {
        SocketData tmp;
        deQueue(cq, &tmp);
    }
}