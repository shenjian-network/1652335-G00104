#include "RPL.h"
static int sfp, nfp;

void RPL_initServer(string (&argvStr)[maxArgc])
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

    cout << "debug1" << endl;
    
    int flag = 1, len = sizeof(int); 
    while(setsockopt(sfp, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1) 
   	{
        if(errno == EINTR)
            continue;
      	perror("setsockopt"); 
     	exit(EXIT_FAILURE);
	}  
	printf("socket ok !\r\n");
	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = htonl(INADDR_ANY);
	s_add.sin_port = htons(portnum);

    cout << "debug2" << endl;

    while(-1 == bind(sfp,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
        if(errno == EINTR)
            continue;
	    printf("bind fail !\r\n");
	    exit(EXIT_FAILURE);
	}
 

    cout << "debug3" << endl;

	while(-1 == listen(sfp,5))
	{
        if(errno == EINTR)
            continue;
	    printf("listen fail !\r\n");
	    exit(EXIT_FAILURE);
	}

    cout << "debug4" << endl;

    sin_size = sizeof(sockaddr_in);
	while(-1 == (nfp = accept(sfp, (sockaddr *)(&c_add), &sin_size)))
	{
        if(errno == EINTR)
            continue;
		printf("accept fail !\r\n");
		exit(EXIT_FAILURE);
	}
	printf("waiting for %#x : %#x\r\n",ntohl(c_add.sin_addr.s_addr),ntohs(c_add.sin_port));
}

void RPL_init_signaction()
{
    PL_init_signaction();
}

void RPL_SendSig_Proc(int RNL_pid)
{
    if(HW_ID == 1)
    {
        kill(RNL_pid, SIG_FRAME_ARRIVAL);
    }
    else if(HW_ID == 2)
    {
        kill(RNL_pid, SIG_FRAME_ARRIVAL);
    }
    else if(HW_ID >= 3)
    {
        double r = dRand();
        if(r < 0.03)
            kill(RNL_pid, SIG_CHK_ERR);
        else if(r < 0.06)
            ;
        else
            kill(RNL_pid, SIG_FRAME_ARRIVAL);
    }
}

void RPL(int* pidArr, string (&argvStr)[maxArgc], int procType)
{
    RPL_init_signaction();
    RPL_initServer(argvStr);
    fd_set readfds,writefds;
    while(1)
    {
        //sleep(1);
        /*
        判断是否有帧到达数据可以读，如果有，发送给SDL一个frame_arriva信号
        判断是否有帧
        */
        FD_ZERO(&readfds);
	    FD_SET(nfp, &readfds);
        writefds=readfds;
	    int select_res = select(nfp + 1, &readfds, &writefds, NULL, NULL);
        if(select_res > 0)
        {
            if(FD_ISSET(nfp,&readfds)){
                preparePLData(nfp, procType);
                //cout << pidArr[1] << endl;
                RPL_SendSig_Proc(pidArr[1]);
            }
            if(FD_ISSET(nfp,&writefds))
            {
                PL_receive_SIG_D2P(nfp, procType);
            }
        }
        //否则是被SDL中断，一种是D2P，就是要写数据，另一种是P2D，就是要收数据
    }
}