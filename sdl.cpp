#include "dl.h"
#include "sdl.h"
#include "common.h"

static int sdl_pid;

static int snl_pid;

static int spl_pid;

static int cnt = 0;

static int cnt_to_phy = 0;

static struct flock lock;

//static char buffer[BLOCK + 1];

static char name[50];

static char name2[50];

static int flag = 0;

static int flag2 = 0;

/*
 *  sdl读了一�?��件，snl才能写入下一�?��两边cnt应�?相等
 *  sdl将�?到的内�?写入到packet里面�?
 */
static void from_network_layer(packet *p)
{
    cout << "receive signal" << endl;
    sprintf(name, "snl.network_datalink.share.%d", cnt);
    while (1)
    {
        int fd = open(name, O_RDONLY);
        if (fd <= 0)
        {
            continue;
        }
        inc(cnt);
        printf("枷锁\n");
        set_lock(fd, F_RDLCK); //如果snl disable，会直接锁�?
        printf("解锁\n");
        memset((char*)(p), 0, sizeof(packet));
        int n_read = myRead(fd, p->data, BLOCK);
        printf("readsize:%d\n",n_read);
        if (n_read < BLOCK)
        {
            cout << "n_read < BLOCK" << endl;
            // 最后一�?���?
            if (!flag)
                flag = !flag;
        }

        set_lock(fd, F_UNLCK);
        close(fd);
        break;
    }
    //memcpy(p->data, buffer, MAX_PKT);
}

/*
 * 主循�?��每�?from_network_layer拿到一�?��后，
 * 直接将所有内容写到文件里面去，写完后向spl发送信号表�?
 */
static void to_physical_layer(frame *f)
{
    sprintf(name2, "sdl.datalink_physical.share.%d", cnt_to_phy);
    int fd = open(name2, O_CREAT | O_WRONLY, 0777);
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
    kill(spl_pid, SIG_D2P);
}

void sdl(int *pidArr)
{
    SDL_init_signaction();
    snl_pid = pidArr[0];
    sdl_pid = pidArr[1];
    spl_pid = pidArr[2];

    frame s;
    packet buffer_s;
    event_type myEvent;
    while (true)
    {
        wait_for_event(&myEvent);
        if(myEvent==network_layer_ready)
        {
            from_network_layer(&buffer_s);
            s.info = buffer_s;
            s.kind=data;
            to_physical_layer(&s);
            if(flag){
                s.kind=data;
                memset((char*)(&s.info),0,sizeof(packet));
                to_physical_layer(&s);
                printf("sdl done\n");
                exit(EXIT_SUCCESS);
            }
        }
        else 
        {
            cout << "ack frame arrival" << endl;    
            kill(snl_pid, SIG_ENABLE_NETWORK_LAYER);    
        }
    }
}