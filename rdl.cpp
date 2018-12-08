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
    sprintf(name1, "rpl.physical_datalink.share.%d", cnt_from_phy);
    int fd = open(name1, O_RDONLY);
    if(fd < 0){
        die("rdl open failed");
    }
    inc(cnt_from_phy);
    int n_read = myRead(fd, f, ACK_SIZE);
    if (f->kind == data)
    {
        n_read+=myRead(fd, &(f->info), MAX_PKT);
    }
    close(fd);
}

void to_network_layer(packet* p){
    sprintf(name2, "rdl.datalink_network.share.%d", cnt_to_net);
    int fd = open(name2, O_CREAT | O_WRONLY, 0777);
    if(fd < 0){
        die("rdl open failed");
    }
    inc(cnt_to_net);
    int n_write = myWrite(fd, p->data, BLOCK);
    close(fd);
    kill(rnl_pid, SIG_NETWORK_READ);
}

bool judgeClose(frame *f)
{
    if(f->kind!=data)
        return true;
    for(int i=0;i<1024;i++)
    {
        if(f->info.data[i]!=0)
            return true;
    }
    return false;
}



void rdl(int* pidArr){

    cout << getpid() << " RDL begin" << endl;
    SDL_init_signaction();

    cout << "init_sig done" << endl;

    rnl_pid = pidArr[0];
    rdl_pid = pidArr[1];
    rpl_pid = pidArr[2];

    frame r;
    event_type event;

    while(true){
        wait_for_event(&event);
        if(event==frame_arrival)
        {
            from_physical_layer(&r);
            if(!judgeClose(&r))
            {
                printf("收到终结文件\n");
                while(1)
                    sleep(1);
            }
            if(r.kind==data)
            {
                to_network_layer(&r.info);
            }
        }
    }    
}