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
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = htonl(INADDR_ANY);
	s_add.sin_port = htons(portnum);
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

void endProcSPL(const int & fd_datalink_physical)
{
    close(fd_datalink_physical);
    close(nfp);
    close(sfp);
    exit(0);
}

void SPL_init_sigaction()
{
    PL_init_signaction();
}

void SPL(int* pidArr, string (&argvStr)[maxArgc])
{
    SPL_init_sigaction();
    SPL_initServer(argvStr);
    socketFd = nfp;
    fd_set readfds;
    while(1)
    {
        //sleep(1);
        /*
        �ж��Ƿ���֡�������ݿ��Զ�������У����͸�SDLһ��frame_arriva�ź�
        �ж��Ƿ���֡
        */
        FD_ZERO(&readfds);
	    FD_SET(nfp, &readfds);
	    int select_res = select(nfp + 1, &readfds, NULL, NULL, NULL);
        if(select_res > 0)
        {
            preparePLData();
            kill(pidArr[1], SIG_FRAME_ARRIVAL);
        }
        //�����Ǳ�SDL�жϣ�һ����D2P������Ҫд���ݣ���һ����P2D������Ҫ������
    }
}