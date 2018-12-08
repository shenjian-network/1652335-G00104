#include "dl.h"
#include "rdl.h"
#include "common.h"

static int rnl_pid;
static int rdl_pid;
static int rpl_pid;

static int cnt_from_phy = 0;
static int cnt_to_net = 0;

static char name1[50];

static char name2[50];

void from_physical_layer(frame* f){
    sprintf(name1, "rdl.datalink_physical.share.%d", cnt_from_phy);
    int fd = open(name1, 0777);
    if(fd < 0){
        die("sdl open failed");
    }

    int n_read = myRead(fd, f, sizeof(frame));

    close(fd);
}

void to_network_layer(packet* p){
    sprintf(name2, "rnl.network_datalink.share.%d", cnt_to_net);
    int fd = open(name2, 0777);
    if(fd < 0){
        die("sdl open failed");
    }

    int n_write = myWrite(fd, p->data, BLOCK);
    close(fd);

    kill(rnl_pid, SIG_D2P);
}



void rdl(int* pidArr){
    rnl_pid = pidArr[0];
    rdl_pid = pidArr[1];
    rpl_pid = pidArr[2];

    frame r;
    event_type event;

    while(true){
        wait_for_event(&event);
        from_physical_layer(&r);
        to_network_layer(&r.info);
    }    
}