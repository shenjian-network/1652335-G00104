#include "RPL.h"
static int cfd;

void RPL_initClient(string (&argvStr)[maxArgc])
{
    //RPL作为client端连接
    struct sockaddr_in s_add,c_add;
    unsigned short portnum = atoi(argvStr[2].c_str());  //port number
    cout << "Ip: " << argvStr[1].c_str() << " Port: " << portnum << endl; 
    cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == cfd)
	{
	    printf("socket fail ! \r\n");
	    exit(EXIT_FAILURE);
	}
    bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = inet_addr(argvStr[1].c_str());
	s_add.sin_port = htons(portnum);
	printf("s_addr = %#x ,port : %#x\r\n", s_add.sin_addr.s_addr, s_add.sin_port);
    while(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
        if(errno == EINTR)
            continue;
	    printf("connect fail !\r\n");
	    exit(EXIT_FAILURE);
	}
	printf("connect ok !\r\n");
}

void RPL_init_signaction()
{
    PL_init_signaction();
}

void RPL(int* pidArr, string (&argvStr)[maxArgc])
{
    RPL_init_signaction();
    RPL_initClient(argvStr);
    socketFd = cfd;
    fd_set readfds,writefds;
    while(1)
    {
        //sleep(1);
        /*
        判断是否有帧到达数据可以读，如果有，发送给SDL一个frame_arriva信号
        判断是否有帧
        */
        FD_ZERO(&readfds);
	    FD_SET(cfd, &readfds);
        writefds=readfds;
	    int select_res = select(cfd + 1, &readfds, &writefds, NULL, NULL);
        if(select_res > 0)
        {
            if(FD_ISSET(cfd,&readfds)){
                preparePLData();
                kill(pidArr[1], SIG_FRAME_ARRIVAL);
            }
            if(FD_ISSET(cfd,&writefds))
            {
                PL_receive_SIG_D2P();
            }
        }
        //否则是被SDL中断，一种是D2P，就是要写数据，另一种是P2D，就是要收数据
    }
}