#include "SPL.h"
static int sfp, nfp;

void SPL_initServer(string (&argvStr)[maxArgc])
{
    struct sockaddr_in s_add, c_add;
    socklen_t sin_size;
    unsigned short portnum = atoi(argvStr[1].c_str());
    sfp = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sfp)
	{
	    printf("socket fail ! \r\n");
	    exit(EXIT_FAILURE);
	}
 
	printf("socket ok !\r\n");
	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr=htonl(INADDR_ANY);
	s_add.sin_port=htons(portnum);
    int flag = 1,len = sizeof(int); 
	if(setsockopt(sfp, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1) 
   	{ 
      	perror("setsockopt"); 
     	exit(EXIT_FAILURE);
	}  

    if(-1 == bind(sfp,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
	    printf("bind fail !\r\n");
	    exit(EXIT_FAILURE);
	}
 
	if(-1 == listen(sfp,5))
	{
	    printf("listen fail !\r\n");
	    exit(EXIT_FAILURE);
	}

    sin_size = sizeof(sockaddr_in);
	nfp = accept(sfp, (sockaddr *)(&c_add), &sin_size);
	if(-1 == nfp)
	{
		printf("accept fail !\r\n");
		exit(EXIT_FAILURE);
	}
	printf("waiting for %#x : %#x\r\n",ntohl(c_add.sin_addr.s_addr),ntohs(c_add.sin_port));
}

void SPL_receive_SIG_D2P(int num, siginfo_t* s, void*)
{
    static int count1 = 0;
    static string file_name_pre = "datalink_physical.share.";
    static char countStr[10];
    sprintf(countStr, "%d", count1);
    string file_name = file_name_pre + countStr;
    count1 = (count1 + 1) % MAX_COUNT1;
    int fd_datalink_physical = open(file_name.c_str(), O_RDONLY);
    if(fd_datalink_physical == -1)
    {
        cerr << "open file " << file_name << " error!";
        return;
    }
    frame frameToSend;
    int size = read(fd_datalink_physical, &frameToSend, sizeof(frameToSend));
    int writebytes;
    switch(frameToSend.kind)
    {
        case data:
            writebytes = write(sfp, &frameToSend, MAX_FRAME_SIZE);
            break;
        case ack:
            writebytes = write(sfp, &frameToSend, ACK_SIZE);
            break;
        case nak:
            writebytes = write(sfp, &frameToSend, NAK_SIZE);
            break;
        default:
            cerr << "datalink_physical_data error!" << endl;
    }
    close(fd_datalink_physical);
}

void SPL_init_signaction()
{
	struct sigaction tact;
	tact.sa_sigaction = SPL_receive_SIG_D2P;
	tact.sa_flags = SA_SIGINFO;
	sigemptyset(&tact.sa_mask);
	sigaction(40, &tact, NULL);
}

void prepareRPLData()
{
    //负责接收数据

}

void SPL(int* pidArr, string (&argvStr)[maxArgc])
{
    SPL_init_signaction();
    SPL_initServer(argvStr);
    fd_set readfds;
    while(1)
    {
        //sleep(1);
        /*判断是否有帧到达数据可以读，如果有，发送给SNL一个frame_arrival即36号信号*/
        FD_ZERO(&readfds);
	    FD_SET(nfp, &readfds);
	    int select_res = select(nfp + 1, &readfds, NULL, NULL, NULL);
        if(select_res > 0)
        {
            kill(pidArr[1], SIG_FRAME_ARRIVAL);
            prepareRPLData();
        }
        //否则是被SDL中断
    }
}