#include "rnl.h"
#include "common.h"

// Data文件的文件描述符
static int dataFd;

// 共享文件的文件描述符
static int fd;

// 读取文件等的buffer
static char buffer[BLOCK + 1];

// 放名字的buffer
static char name[50];

// 计数器
static int cnt = 0;


/*
 * 一旦收到38信号，读文件，然后追加写入
 */
static void receive_sig38(int signum){
    sprintf(name, "rnl.network_datalink.share.%d", cnt);
    int fd = open(name, O_RDONLY);
    if(fd < 0){
        die("rnl open failed");
    }
    
    inc(cnt); //打开成功再inc

    int n_read = read(fd, buffer, BLOCK);
    if(n_read < 0){
        die("rnl read failed");
    }
    close(fd);

    int n_write = write(dataFd, buffer, n_read);
    if(n_write < 0){
        die("rnl write failed");
    }
}  



void rnl(int* pidArr){
    dataFd = open("output", 0777);

    if(dataFd < 0){
        die(strerror(errno));
    }

     // 注册
    signal(38, receive_sig38);

    // 主循环
    while(true){
        sleep(1);
    }

    close(dataFd);
}