#ifndef COMMON_H_
#define COMMON_H_

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>  


#define ENABLE 1
#define DISALBE 0

#define BLOCK 1024

// 错误退出
void die(const char* msg);

#endif