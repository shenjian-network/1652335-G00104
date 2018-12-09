#include "dl.h"
#include "rdl.h"
#include "common.h"

static int rnl_pid;
static int rdl_pid;
static int rpl_pid;
static int begin_flag = 0;
static int cnt_from_phy = 0;
static int cnt_to_net = 0;
static int cnt_to_phy = 0;
static char name1[50];

static char name2[50];

static char name3[50];

void from_physical_layer(frame *f)
{
    printf("%d\n",cnt_from_phy);
    sprintf(name1, "rpl.physical_datalink.share.%d", cnt_from_phy);
    int fd = open(name1, O_RDONLY);
    if (fd < 0)
    {
        die("rdl open failed1");
    }
    inc(cnt_from_phy);
    int n_read = myRead(fd, f, ACK_SIZE);
    if (f->kind == data)
    {
        n_read += myRead(fd, &(f->info), MAX_PKT);
    }
    close(fd);
}

void to_network_layer(char* myBuffer,int size=BLOCK)
{
    sprintf(name2, "rdl.datalink_network.share.%d", cnt_to_net);
    int fd = open(name2, O_CREAT | O_TRUNC|O_WRONLY, 0777);
    if (fd < 0)
    {
        die("rdl open failed2");
    }
    inc(cnt_to_net);
    int n_write = myWrite(fd, (void*)myBuffer, size);
    close(fd);
    kill(rnl_pid, SIG_NETWORK_READ);
}

bool judgeClose(frame *f)
{
    if (f->kind != data)
        return true;
    for (int i = 0; i < 1024; i++)
    {
        if (f->info.data[i] != 0)
            return true;
    }
    return false;
}

static void to_physical_layer(frame *f)
{
    sprintf(name3, "rdl.datalink_physical.share.%d", cnt_to_phy);
    int fd = open(name3, O_CREAT | O_WRONLY | O_TRUNC, 0777);
    if (fd < 0)
    {
        die("sdl open failed");
    }
    inc(cnt_to_phy);
    int n_write = myWrite(fd, (void *)f, sizeof(frame));
    if (n_write < 0)
    {
        die("sdl write failed");
    }
    close(fd);

    kill(rpl_pid, SIG_D2P);
}

void rdl(int *pidArr)
{
    char data_tmp_buffer[1024];
    cout << getpid() << " RDL begin" << endl;
    SDL_init_signaction();

    cout << "init_sig done" << endl;

    rnl_pid = pidArr[0];
    rdl_pid = pidArr[1];
    rpl_pid = pidArr[2];

    frame r;
    frame S;
    memset(&S, 0, sizeof(frame));
    S.kind = ack;
    S.seq = 0xffffffffu;
    event_type event;

    while (true)
    {
        wait_for_event(&event);
        if (event == frame_arrival)
        {
            from_physical_layer(&r);
            if (!judgeClose(&r))
            {
                int s=1023;
                for(;s>=0&&data_tmp_buffer[s]==0;s--);
                s++;
                to_network_layer(data_tmp_buffer,s);
                printf("all end\n");
                to_physical_layer(&S);
                while (1)
                    sleep(1);
            }
            if (r.kind == data)
            {
                if (!begin_flag)
                {
                    begin_flag = 1;
                    memcpy(data_tmp_buffer,&r.info,1024);
                    to_physical_layer(&S);
                    continue;
                }
                to_network_layer(data_tmp_buffer);
                memcpy(data_tmp_buffer,&r.info,1024);
                to_physical_layer(&S);
            }
        }
    }
}