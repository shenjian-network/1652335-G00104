#include "rnl.h"

// Data文件的文件描述符
static int dataFd;

// 共享文件的文件描述符
static int fd;

// 读取文件等的buffer
static char buffer[BLOCK + 1];

// 读取文件等的buffer
static char buffer_r[BLOCK + 1];

// 放名字的buffer
static char name[50];

// 放名字的buffer
static char name_r[50];

// 计数器
static int cnt = 0;

// 计数器
static int cnt_r = 0;




/*
 * 一旦收到38信号，读文件，然后追加写入
 */
static void receive_sig38(int signum){//留待更改
    sprintf(name, "rnl.network_datalink.share.%d", cnt);
    int fd = open(name, O_RDONLY);
    if(fd < 0){
        die("rnl open failed");
    }
    
    inc(cnt); //打开成功再inc

    int n_read = myRead(fd, buffer, BLOCK);
    if(n_read < 0){
        die("rnl read failed");
    }
    close(fd);

    int n_write = myWrite(dataFd, buffer, n_read);
    if(n_write < 0){
        die("rnl write failed");
    }
}  

static void receive_datalink(int sigv)
{
    sprintf(name_r, "rdl.datalink_network.share.%d", cnt_r);
    int fd = open(name_r, O_RDONLY);
    if(fd < 0){
        die("rnl open failed");
    }
    inc(cnt_r); //打开成功再inc

    int n_read = myRead(fd, buffer_r, BLOCK);
    if(n_read < 0){
        die("rnl read failed");
    }
    close(fd);

    int n_write = myWrite(dataFd, buffer_r, n_read);
    if(n_write < 0){
        die("rnl write failed");
    }   
}


void rnl(int* pidArr, std::string (&argvStr)[maxArgc]){
    // 注册
    signal(SIG_ENABLE_NETWORK_LAYER, receive_sig38);
    signal(SIG_NETWORK_READ, receive_datalink);
    
    dataFd = open(argvStr[3].c_str(), O_CREAT | O_TRUNC | O_RDWR, 0777);

    if(dataFd < 0){
        die(strerror(errno));
    }

    // 主循环
    while(true){
        sleep(1);
    }

    close(dataFd);
}