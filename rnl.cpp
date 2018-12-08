#include "rnl.h"
#include "common.h"

// RNL对应的进程号
static pid_t rnl_pid;

static pid_t rdl_pid;

// 默认为1
static bool network_enabled = ENABLE;

// Data文件的文件描述符
static int dataFd;

// 共享文件的文件描述符
static int fd;

// 读取文件等的buffer
static char buffer[BLOCK + 1];

// 放名字的buffer
static char name[50];

// 计数器
static int count = 1;


static int flag = 0;

static void enable_network_layer(){
    network_enabled = ENABLE;
}

static void disable_network_layer(){
    network_enabled = DISALBE;
}


static void create_file(){
    if(flag){
        return;
    }
    
    int n_read = read(dataFd, buffer, BLOCK);
    
    if(n_read < 0){
        die("read failed");
    }

    if(n_read < BLOCK){
        // 最后一个文件
        if(!flag)
            flag = !flag;
    }

    sprintf(name, "network_datalink.share.%d", count);
    inc(count);
    fd = creat(name, 0777);

    if(fd < 0){
        die("open failed");
    }

    int n_write = write(fd, buffer, n_read);

    if(n_write < 0 || n_write != n_read){
        die("write failed"); 
    }
    
    close(fd);
}

static void receive_sig38(int signum){
    // 判断文件是否存在
    printf("%d\n", count);
    sprintf(name, "network_datalink.share.%d", count);
    if(access(name, F_OK) != -1){
        // 之前一定是DISABLE 且 一定上过锁
        if(network_enabled == DISALBE){
            set_lock(fd, F_UNLCK);          
        }
    }

    network_enabled = ENABLE;

    create_file();

    // 向SDL发送37信号
    kill(rdl_pid, 37);
}  


static void receive_sig39(int signum){
    network_enabled = DISALBE;
    set_lock(fd, F_WRLCK);          
}



void snl(int* pidArr){
    rdl_pid = pidArr[1];
    rnl_pid = pidArr[0];
    dataFd = open("jd.html", O_RDONLY);

    if(dataFd < 0){
        die(strerror(errno));
    }

     // 注册
    signal(38, receive_sig38);
    signal(39, receive_sig39);

    // 一开始要主动调用一次
    receive_sig38(0);
    // 主循环
    while(true){
        sleep(1);
    }
}