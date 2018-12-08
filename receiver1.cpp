#include "mainProc.h"
#include "RPL.h"
#include "rnl.h"
#include "rdl.h"


void forkSender(int & shmid, procState & procS, string (&argvStr)[maxArgc], bool procType)
{
	shmid = createShm(1024);
	int *pidArr = (int*)shmat(shmid, NULL, 0);
	
	pid_t pid_1 = fork();
	if(pid_1 == 0)
	{
		pid_t pid_2 = fork();
		if(pid_2 == 0)
		{
			procS = eNL;
			prepareProc(pidArr, procS, shmid, procType);
			rnl(pidArr);
			exit(0);
		}
		else
		{
			procS = ePL;
			prepareProc(pidArr, procS, shmid, procType);
			RPL(pidArr, argvStr);
			exit(0);
		}
	}
	else
	{
		procS = eDL;
		prepareProc(pidArr, procS, shmid, procType);
		rdl(pidArr);
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	procType = 1;

	PROJ_ID = procType ? 666 : 233;

	checkArgc(argc, procType);
	
	createDaemon();

	string argvStr[maxArgc];
	for(int i = 0; i < argc; i++)
		argvStr[i] = string(argv[i]);
	
	setproctitle_init(argc, argv, environ);

	int shmid;
	procState procS;
	forkSender(shmid, procS, argvStr, procType);

	workDone(shmid, procS);
}