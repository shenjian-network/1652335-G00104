#include "PL.h"
static int countRecv;

void preparePLData()
{
    /*
    PL:�ӶԶ˽������ݣ��������ļ���
    */
    static string plStr[2] = {string("spl"), string("sdl")};
    static string file_name_pre = plStr[procType] + ".physical_datalink.share.";
    static char countStr[10];
    sprintf(countStr, "%d", countRecv);
    string file_name = file_name_pre + countStr;
    countRecv = (countRecv + 1) % MAX_COUNT;
    int fd_recv = open(file_name.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);

    /*�ӶԶ˽�������*/
    frame frameRecv;
    int size = myRead(socketFd, &frameRecv, ACK_SIZE);
    if(frameRecv.kind == data)
        size += myRead(socketFd, &(frameRecv.info), MAX_PKT);
    /*�����ݷ����ļ�*/
    myWrite(fd_recv, &frameRecv, size);
    close(fd_recv);
}

//to_physical_layer����д���ݣ�֮��ͻ�kill(PL, SIG_D2P)
void PL_receive_SIG_D2P(int, siginfo_t*, void*)//�յ�SIG_D2P�źţ����õ������ݷ���
{
    static int count_D2P = 0;
    static string plStr[2] = {string("spl"), string("sdl")};
    static string file_name_pre = plStr[procType] + ".datalink_physical.share.";
    static char countStr[10];
    sprintf(countStr, "%d", count_D2P);
    string file_name = file_name_pre + countStr;
    count_D2P = (count_D2P + 1) % MAX_COUNT;
    int fd_datalink_physical = open(file_name.c_str(), O_RDONLY);
    if(fd_datalink_physical == -1)
    {
        cerr << "open file " << file_name << " error!";
        return;
    }
    frame frameToSend;

    int size = myRead(fd_datalink_physical, &frameToSend, sizeof(frameToSend));

    int writebytes;
    switch(frameToSend.kind)
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

void PL_init_signaction()
{
	struct sigaction tact;
	tact.sa_sigaction = PL_receive_SIG_D2P;
	tact.sa_flags = SA_SIGINFO;
	sigemptyset(&tact.sa_mask);
	sigaction(SIG_D2P, &tact, NULL);
}