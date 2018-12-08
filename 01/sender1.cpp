#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "common.h"
#include "SPL.h"

using namespace std;

const int HW_ID = 1;
const int PROJ_ID = 233;
const char * PATHNAME = ".";
extern char **environ;
static char **g_main_Argv = NULL; /* pointer to argument vector */
static char *g_main_LastArgv = NULL; /* end of argv */

enum procState
{
	eSNL,
	eSDL,
	eSPL
};

void createDaemon()
{
	umask(0);
	int i;
	int fd0;
	pid_t pid, pid1;
	if ((pid = fork()) < 0)
		perror("Fork Error");
	else if (pid)
		exit(0);
	setsid();
	signal(SIGCHLD, SIG_IGN);

	if ((pid1 = fork()) < 0)
		perror("Fork Error!");
	else if (pid1)
		exit(0);
}

void setproctitle_init(int argc, char **argv, char **envp)
{
    int i;
    for (i = 0; envp[i] != NULL; i++) // calc envp num
        continue;
    environ = (char **) malloc(sizeof (char *) * (i + 1)); // malloc envp pointer

    for (i = 0; envp[i] != NULL; i++)
    {
        environ[i] = (char *)malloc(sizeof(char) * strlen(envp[i]));
        strcpy(environ[i], envp[i]);
    }
    environ[i] = NULL;
    g_main_Argv = argv;
    if (i > 0)
      g_main_LastArgv = envp[i - 1] + strlen(envp[i - 1]);
    else
      g_main_LastArgv = argv[argc - 1] + strlen(argv[argc - 1]);
}

void setproctitle(const char *fmt, ...)
{
    char *p;
    int i;
    char buf[1024];
    extern char **g_main_Argv;
    extern char *g_main_LastArgv;
    va_list ap;
    p = buf;
    va_start(ap, fmt);
    vsprintf(p, fmt, ap);
    va_end(ap);
    i = strlen(buf);
    if (i > g_main_LastArgv - g_main_Argv[0] - 2)
    {
        i = g_main_LastArgv - g_main_Argv[0] - 2;
        buf[i] = '\0';
    }
    (void) strcpy(g_main_Argv[0], buf);
    p = &g_main_Argv[0][i];
    while (p < g_main_LastArgv)
        *p++ = '\0';
    g_main_Argv[1] = NULL;
    prctl(PR_SET_NAME, buf);
}

static int commShm(int size, int shmflag)
{
	key_t key = ftok(PATHNAME, PROJ_ID); // 获取key 
	if(key == -1){
		perror("ftok");
		return -1;
	}
	int shmid = shmget(key,  size, shmflag);
	if(shmid < 0){
		perror("shmget");
		return -2;
	}
	return shmid;
}

int createShm(int size)
{
	return commShm(size, IPC_CREAT|0666);
}

int getShmid()
{
	return commShm(0, IPC_CREAT);
}

int destroyShm(int shmid)
{
	if(shmctl(shmid, IPC_RMID, NULL) < 0)
	{
		perror("shmctl");
		return -1;
	}
	return 0;
}

void workDone(const int shmid, const procState & procS)
{
	if(procS == eSDL)
	{
		destroyShm(shmid);
		for (int i = 0; environ[i] != NULL; i++)
        	free(environ[i]);
	}
	exit(0);
}

void prepareProc(int *pidArr, const procState & procS, const int shmid)
{
	createDaemon();
	pidArr[procS] = getpid();
	switch(procS)
	{
		case eSNL:
			setproctitle("%s%d%s", "send", HW_ID, "_network");
			break;
		case eSDL:
			setproctitle("%s%d%s", "send", HW_ID, "_datalink");
			break;
		case eSPL:
			setproctitle("%s%d%s", "send", HW_ID, "_physical");
			break;
		default:
			cerr << "procS error!" << endl;
			workDone(shmid, procS);
	}
	sleep(1);
}

/*-----------*/
void SNL(int* pidArr)
{
	while(1)
		;
}
void SDL(int* pidArr)
{
	while(1)
		;
}
/*-----------*/

void forkSender(int & shmid, procState & procS, string (&argvStr)[maxArgc])
{
	shmid = createShm(1024);
	int *pidArr = (int*)shmat(shmid, NULL, 0);
	
	pid_t pid_1 = fork();
	if(pid_1 == 0)
	{
		pid_t pid_2 = fork();
		if(pid_2 == 0)
		{
			procS = eSNL;
			prepareProc(pidArr, procS, shmid);
			SNL(pidArr);
			exit(0);
		}
		else
		{
			procS = eSPL;
			prepareProc(pidArr, procS, shmid);
			SPL(pidArr, argvStr);
			exit(0);
		}
	}
	else
	{
		procS = eSDL;
		prepareProc(pidArr, procS, shmid);
		SDL(pidArr);
		exit(0);
	}
}

void checkArgc(int argc)
{
	if(argc != 2)
	{
		cerr << "参数个数不对" << endl;
		exit(1);
	}
}
int main(int argc, char* argv[])
{
	checkArgc(argc);
	
	createDaemon();

	string argvStr[maxArgc];
	for(int i = 0; i < argc; i++)
		argvStr[i] = string(argv[i]);
	
	setproctitle_init(argc, argv, environ);

	int shmid;
	procState procS;
	forkSender(shmid, procS, argvStr);

	workDone(shmid, procS);
}