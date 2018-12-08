#include "sdl.h"
#include "common.h"

static int sdl_pid;

static int snl_pid;

static int spl_pid;

static int cnt = 1;

static struct flock lock;

static char buffer[BLOCK + 1];

static char name[50];

static int flag = 0;

bool operator==(const time_node a, const seq_nr k)
{
	return a.k == k;
}

/*
 *  sdl读了一个文件，snl才能写入下一个，两边cnt应该相等
 *  sdl将读到的内容写入到packet里面，
 */
static void from_network_layer(packet* p){
    sprintf(name, "network_datalink.share.%d", cnt);
    int fd = open(name, O_RDONLY);
    if(fd < 0){
        return;
    }

    inc(cnt);
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

static int tout_ack=-1;
static void start_ack_timer()
{
    tout_ack=timeout_ack_interval;
}

static void stop_ack_timer() 
{
    tout_ack=-1;
}

static vector<time_node> timeout_list;
static void start_timer(seq_nr k) 
{
    time_node tn;
    tn.k=k;
    tn.tout=timeout_interval;
    for(vector<time_node>::iterator tnode=timeout_list.begin();tnode!=timeout_list.end();tnode++)
    {
        tn.tout-=tnode->tout;
    }
    timeout_list.push_back(tn);
    return;
}

static void stop_timer(seq_nr k) 
{
    vector<time_node>::iterator ti;
    ti=find(timeout_list.begin(),timeout_list.end(),k);
    if(ti==timeout_list.end())
        return;
    for(vector<time_node>::iterator tmp=ti+1;tmp!=timeout_list.end();tmp++)
    {
        tmp->tout+=ti->tout;
    }
    timeout_list.erase(ti);
    return;
}

static queue<event_type> event_list;
static queue<seq_nr> oldest_frame_list;
static void sigHandle(int sigv){
    vector<time_node>::iterator ti=timeout_list.begin();
    switch(sigv){
        case SIG_CHK_ERR: 
            event_list.push(cksum_err);
        break;
        case SIG_FRAME_ARRIVAL:
            event_list.push(frame_arrival);
        break;
        case SIG_NETWORK_LAYER_READY: 
            event_list.push(network_layer_ready);
        break;
        case SIGALRM:
            if(tout_ack!=-1)
            {
                --tout_ack;
                if(tout_ack==0)
                {
                    stop_ack_timer();
                    event_list.push(ack_timeout);
                }
            }
            if(ti!=timeout_list.end())
            {
                ti->tout-=1;
                if(ti->tout<=0)
                {
                    event_list.push(timeout);
                    oldest_frame_list.push(ti->k);
                    timeout_list.erase(ti);
                }
            }
        break;
        default: 
        break;
    }
    return;
}

static seq_nr oldest_frame;
static void wait_for_event(event_type* event)
{
    while(1)
    {
        if(event_list.empty())
        {
            sleep(100000);
            continue;
        }
        (*event)=event_list.front();
        if((*event)==timeout)
        {
            oldest_frame=oldest_frame_list.front();
            oldest_frame_list.pop();
        }
        event_list.pop();
        break;
    }
    return;
}

static void SDL_init_signaction()
{
    signal(SIG_CHK_ERR,sigHandle);
    signal(SIG_FRAME_ARRIVAL,sigHandle);
    signal(SIG_NETWORK_LAYER_READY,sigHandle);
    signal(SIGALRM,sigHandle);
    struct itimerval one_timer;
	one_timer.it_interval.tv_sec=0;
    one_timer.it_interval.tv_usec=1;
	one_timer.it_value.tv_sec=0;
    one_timer.it_value.tv_usec=1;
	setitimer(ITIMER_REAL,&one_timer,NULL);
}


void sdl(int* pidArr){
    //SDL_init_signaction();
    //start_timer(1);
    //start_timer(2);
    //stop_timer(2);
    //event_type myevent;
    //while(1)
    //{
      //  wait_for_event(&myevent);
    //}

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
