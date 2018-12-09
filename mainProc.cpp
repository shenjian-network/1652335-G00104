#include "mainProc.h"
const char * const PATHNAME = ".";
void workDone(const int shmid, const procState & procS)
{
	if(procS == eDL)
	{
		cout << "workDone" << endl;
		destroyShm(shmid);
		for (int i = 0; environ[i] != NULL; i++)
        	free(environ[i]);
	}
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
        environ[i] = (char *)malloc(sizeof(char) * (strlen(envp[i]) + 1));
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
	key_t key = PROJ_ID; //È¡key 
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

void prepareProc(int *pidArr, const procState & procS, const int shmid, bool procType)
{
	createDaemon();
	pidArr[procS] = getpid();
    string procTypeStr = procType == 0 ? string("send") : string("receiver");
	cout << "setproctitle " << procS << " begin" << endl;
	switch(procS)
	{
		case eNL:
			setproctitle("%s%d%s", procTypeStr.c_str(), HW_ID, "_network");
			break;
		case eDL:
			setproctitle("%s%d%s", procTypeStr.c_str(), HW_ID, "_datalink");
			break;
		case ePL:
			setproctitle("%s%d%s", procTypeStr.c_str(), HW_ID, "_physical");
			break;
		default:
			cerr << "procS error!" << endl;
			workDone(shmid, procS);
	}
	cout << "setproctitle " << procS << " done" << endl;
	sleep(1 + procS);
}

void checkArgc(int argc, bool procType)
{
	if((procType == 0 && argc != 4) || (procType == 1 && argc != 3))
	{
		cerr << "argument number is wrong" << endl;
		exit(1);
	}	
}

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