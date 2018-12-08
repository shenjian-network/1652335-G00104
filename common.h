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

const int SIG_CHK_ERR = 35;
const int SIG_FRAME_ARRIVAL = 36;
const int SIG_NETWORK_LAYER_READY = 37;
const int SIG_ENABLE_NETWORK_LAYER = 38;
const int SIG_DISABLE_NETWORK_LAYER = 39;
const int SIG_D2P = 40;
const int maxArgc = 20;
#define ENABLE 1
#define DISALBE 0
#define BLOCK 1024
#define  MAX_PKT  1024

#define MAX_SEQ 10000
#define inc(k) if(k<MAX_SEQ) k=k+1; else k=1;

void set_lock(int fd, int type=F_WRLCK);

// 错误退出
void die(const char* msg);

// 读入数据
int myRead(int fd,void* buffer,int size);

// 写数据
int myWrite(int fd,void* buffer,int size);

typedef unsigned int seq_nr;    //发送序号
typedef struct {unsigned char data[MAX_PKT];} packet; //数据包，纯数据
typedef enum {
    data, //数据包
    ack,  //确认包
    nak   //否定确认包
} frame_kind; //帧类型枚举量
typedef struct {
    frame_kind kind; //帧类型
    seq_nr seq;  //发送序号
    seq_nr ack;  //接收序号
    packet     info; //数据包
} frame; //帧结构
typedef enum
{
    frame_arrival,
    cksum_err,
    timeout,
    network_layer_ready,
    ack_timeout
}event_type;

#endif