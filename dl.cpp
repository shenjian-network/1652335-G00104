#include "dl.h"

bool operator==(const time_node a, const seq_nr k)
{
	return a.k == k;
}


int tout_ack=-1;
void start_ack_timer()
{
    tout_ack=timeout_ack_interval;
}

void stop_ack_timer() 
{
    tout_ack=-1;
}

vector<time_node> timeout_list;
void start_timer(seq_nr k) 
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

void stop_timer(seq_nr k) 
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

queue<event_type> event_list;
queue<seq_nr> oldest_frame_list;
void sigHandle(int sigv){
    vector<time_node>::iterator ti=timeout_list.begin();
    switch(sigv){
        case SIG_CHK_ERR: 
            event_list.push(cksum_err);
        break;
        case SIG_FRAME_ARRIVAL:
            cout << "SIG_FRAM_ARRIVAL" << endl;
            event_list.push(frame_arrival);
        break;
        case SIG_NETWORK_LAYER_READY: 
            cout << "SIG_NETWORK_LAYER_READY" << endl;
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

seq_nr oldest_frame;
void wait_for_event(event_type* event)
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



void SDL_init_signaction()
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
	//setitimer(ITIMER_REAL,&one_timer,NULL);
}
