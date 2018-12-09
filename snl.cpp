#include "snl.h"

// SNL对应的进程号
static pid_t snl_pid;

static pid_t sdl_pid;

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
static int count = 0;


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
    
    int n_read = myRead(dataFd, buffer, BLOCK);
    
    if(n_read < BLOCK){
        // 最后一个文件
        if(!flag)
        {
            printf("snl read done\n");
            flag = !flag;
        }
    }

    sprintf(name, "snl.network_datalink.share.%d", count);
    inc(count);
    fd = creat(name, S_IRUSR | S_IWUSR);

    if(fd < 0){
        die("open failed");
    }
    // printf("add lock begin write\n");
    set_lock(fd, F_WRLCK);
    int n_write = myWrite(fd, buffer, n_read);
    set_lock(fd, F_UNLCK);

    if(n_write < 0 || n_write != n_read){
        die("write failed"); 
    }
    
    close(fd);
}

static void receive_sig38(int signum){
    // 判断文件是否存在
    sprintf(name, "snl.network_datalink.share.%d", count);
    if(access(name, F_OK) != -1){
        // 之前一定是DISABLE 且 一定上过锁
        if(network_enabled == DISALBE){
            set_lock(fd, F_UNLCK);          
        }
    }

    network_enabled = ENABLE;

    create_file();

    // 向SDL发送37信号
    kill(sdl_pid, 37);
}  


static void receive_sig39(int signum){
    network_enabled = DISALBE;
    set_lock(fd, F_WRLCK);          
}

static int snlStart=0;
static void StartHandle(int sigv){
    snlStart=1;
}

void snl(int* pidArr, std::string (&argvStr)[maxArgc]){
    // 注册
    signal(38, receive_sig38);
    signal(38, receive_sig38);
    signal(SIG_ALL_START, StartHandle);
    while(1)
    {
        if(snlStart)
            break;
        else{
            sleep(1);
        }
    }
    sdl_pid = pidArr[1];
    snl_pid = pidArr[0];
    dataFd = open(argvStr[2].c_str(), O_RDONLY);

    if(dataFd < 0){
        die(strerror(errno));
    }

    // 一开始要主动调用一次
    receive_sig38(0);                                                                                               
    // 主循环
    while(true){
        sleep(1);
    }
}