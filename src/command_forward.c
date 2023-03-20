/*
 * @Author       : iPEK
 * @Date         : 2023-03-02
 * @LastEditTime : 2023-03-10
 * @Description  : main函数入口
 *
 * Copyright (c) 2023 by iPEK, All Rights Reserved.
 */

#include "../include/socket_forward/my_socket.h"

int main()
{
    setbuf(stdout, NULL);
    initSocket();
    listenAndForward();
}