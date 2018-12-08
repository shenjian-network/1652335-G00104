#include "sdl.h"
#include "../common/common.h"


static int sdl_pid;

static int snl_pid;

static int spl_pid;

static int cnt_from_net = 1;

static int cnt_to_phy = 1;

static struct flock lock;

static char buffer[BLOCK + 1];

static char name1[50];

static char name2[50];

static int flag = 0;


/*
 *  sdl读了一个文件，snl才能写入下一个，两边count应该相等
 *  sdl将读到的内容写入到packet里面，
 */
static void from_network_layer(packet* p){
    sprintf(name1, "snl.network_datalink.share.%d", cnt_from_net);
    int fd = open(name1, O_RDONLY);
    if(fd < 0){
        return;
    }

    inc(cnt_from_net);
    set_lock(fd, F_RDLCK); //如果snl disable，会直接锁死

    int n_read = read(fd, buffer, BLOCK);
    if(n_read < 0){
        die("sdl read failed");
    }

   
    set_lock(fd, F_UNLCK);
    close(fd);

    memcpy(p->data, buffer, MAX_PKT);
    kill(snl_pid, 38);
}

/*
 * 主循环，每次from_network_layer拿到一个帧后，
 * 直接将所有内容写到文件里面去，写完后向spl发送信号表示
 */
static void to_physical_layer(frame* f){
    sprintf(name2, "sdl.datalink_physical.share.%d", cnt_to_phy);
    int fd = open(name2, 0777);
    if(fd < 0){
        die("sdl open failed");
    }

    int n_write = write(fd, f, sizeof(frame));
    if(n_write < 0){
        die("sdl write failed");
    }
    close(fd);

    kill(spl_pid, SIG_D2P);
}

/*
 * 好像没用，但是删了好像有事
 */
static void receive_sig37(int signum){
    // TODO 
    return;
}


void sdl(int* pidArr){
    snl_pid = pidArr[0];
    sdl_pid = pidArr[1];
    spl_pid = pidArr[2];

    signal(37, receive_sig37);
    signal(36, receive_sig37);
    signal(35, receive_sig37);

    frame s;
    packet buffer;

    while(true){
        from_network_layer(&buffer);
        s.info = buffer;
        to_physical_layer(&s);
    }
}