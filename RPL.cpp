#include "RPL.h"
static int cfd;

void RPL_initClient(string (&argvStr)[maxArgc])
{
    //RPL��Ϊclient������
    struct sockaddr_in s_add,c_add;
    unsigned short portnum = atoi(argvStr[2].c_str());  //port number
    cout << "Ip: " << argvStr[2].c_str() << " Port: " << portnum << endl; 
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
    if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
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
    fd_set readfds;
    while(1)
    {
        //sleep(1);
        /*
        �ж��Ƿ���֡�������ݿ��Զ�������У����͸�SDLһ��frame_arriva�ź�
        �ж��Ƿ���֡
        */
        FD_ZERO(&readfds);
	    FD_SET(cfd, &readfds);
	    int select_res = select(cfd + 1, &readfds, NULL, NULL, NULL);
        if(select_res > 0)
        {
            preparePLData();
            kill(pidArr[1], SIG_FRAME_ARRIVAL);
        }
        //�����Ǳ�SDL�жϣ�һ����D2P������Ҫд���ݣ���һ����P2D������Ҫ������
    }
}