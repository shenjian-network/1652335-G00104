#include "sdl.h"
#include "common.h"

static int sdl_pid;

static int snl_pid;

static int spl_pid;

static int count = 0;

static struct flock lock;

static char buffer[BLOCK + 1];

static char name[50];


static void from_network_layer(packet* p){
    inc(count);

    sprintf(name, "network_datalink.share.%d", count);
    int fd = open(name, O_RDONLY);

    set_lock(fd, F_RDLCK); //如果snl disable，会直接锁死

    int n_read = read(fd, buffer, BLOCK);
    if(n_read < 0){
        die("read failed");
    }

    set_lock(fd, F_UNLCK);
    kill(snl_pid, 38);
}


void sdl(int* pidArr){
    snl_pid = pidArr[0];
    sdl_pid = pidArr[1];
    spl_pid = pidArr[2];

    while(1){
        sleep(1);
        packet p;
        from_network_layer(&p);
        printf("count %d", count);
        
    }
}