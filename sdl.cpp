#include "sdl.h"
#include "common.h"

static int sdl_pid;

static int snl_pid;

static int spl_pid;

static int count = 1;

static struct flock lock;

static char buffer[BLOCK + 1];

static char name[50];

static int flag = 0;


/*
 *  sdl读了一个文件，snl才能写入下一个，两边count应该相等
 *  sdl将读到的内容写入到packet里面，
 */
static void from_network_layer(packet* p){
    sprintf(name, "network_datalink.share.%d", count);
    int fd = open(name, O_RDONLY);
    if(fd < 0){
        return;
    }

    inc(count);
    set_lock(fd, F_RDLCK); //如果snl disable，会直接锁死

    int n_read = read(fd, buffer, BLOCK);
    if(n_read < 0){
        die("read failed");
    }

   
    set_lock(fd, F_UNLCK);
    close(fd);

    memcpy(p->data, buffer, MAX_PKT);
    kill(snl_pid, 38);
}


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

    /*
    // TEST from_network_layer
    while(1){
        packet p;
        p.data[0] = '\0';
        from_network_layer(&p);
        if(p.data[0] == '\0')
            sleep(1);
        for(int i = 0; i < MAX_PKT; ++i)
            putchar(p.data[i]);
        putchar('\n');
    }*/
}