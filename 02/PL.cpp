#include "PL.h"
static int countRecv;
static int countReady = 0;

void disconnectSuspend()
{
    while(1)
        sleep(1);
}

void SIG_D2P_Handle(int sigv)
{
    // printf("1\n");
    countReady++;
    return;
}

void delay()
{
    sigset_t blockset;
    sigemptyset(&blockset);
    sigaddset(&blockset, SIG_D2P);
    sigprocmask(SIG_BLOCK, &blockset,NULL);
    sleep(1);
    sigprocmask(SIG_UNBLOCK, &blockset,NULL);
}   

void preparePLData(int socketFd, int procType)
{
    /*
    PL:从对端接收数据，并放入文件中
    */
    static string plStr[2] = {string("spl"), string("rpl")};
    static string file_name_pre = plStr[procType] + ".physical_datalink.share.";
    static char countStr[10];
    sprintf(countStr, "%d", countRecv);
    string file_name = file_name_pre + countStr;
    countRecv = (countRecv + 1) % MAX_COUNT;
    int fd_recv = open(file_name.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);

    /*从对端接收数据*/
    frame frameRecv;
    int size = myRead(socketFd, &frameRecv, ACK_SIZE);

    if(size == 0)
        disconnectSuspend();

    if (frameRecv.kind == data)
        size += myRead(socketFd, &(frameRecv.info), MAX_PKT);
    
    else
    {  
        if(frameRecv.kind == ack && dRand() < 0.1)
        {
            cout << "delay" << endl;
            delay();
        }
    }
    
    /*将数据放入文件*/
    myWrite(fd_recv, &frameRecv, size);
    close(fd_recv);
}

//to_physical_layer就是写数据，之后就会kill(PL, SIG_D2P)
void PL_receive_SIG_D2P(int socketFd, int procType)
{
    if (countReady > 0)
    {
        // cout << "countReady:" << countReady << endl;
        countReady--;
        static int count_D2P = 0;
        static string plStr[2] = {string("sdl"), string("rdl")};
        static string file_name_pre = plStr[procType] + ".datalink_physical.share.";
        static char countStr[10];
        sprintf(countStr, "%d", count_D2P);
        string file_name = file_name_pre + countStr;
        count_D2P = (count_D2P + 1) % MAX_COUNT;
        int fd_datalink_physical = open(file_name.c_str(), O_RDONLY);
        if (fd_datalink_physical == -1)
        {
            cerr << "open file " << file_name << " error!" << endl;
            return;
        }
        frame frameToSend;
        int size = myRead(fd_datalink_physical, &frameToSend, sizeof(frameToSend));
        int writebytes;
        switch (frameToSend.kind)
        {
        case data:
            writebytes = myWrite(socketFd, &frameToSend, MAX_FRAME_SIZE);
            break;
        case ack:
            writebytes = myWrite(socketFd, &frameToSend, ACK_SIZE);
            break;
        case nak:
            writebytes = myWrite(socketFd, &frameToSend, NAK_SIZE);
            break;
        default:
            cerr << "datalink_physical_data error!" << endl;
        }
        close(fd_datalink_physical);
    }
}

static void setMask(int sigv,void(*si)(int x))
{
    struct sigaction newact, oldact;
	newact.sa_handler = si;
	sigemptyset(&newact.sa_mask);
	sigaddset(&newact.sa_mask, SIG_D2P);
	newact.sa_flags = 0;
    sigaction(sigv, &newact, &oldact);
}

void PL_init_signaction()
{
    setMask(SIG_D2P, SIG_D2P_Handle);
}

double dRand()
{
    return 1.0 * rand() / RAND_MAX;
}