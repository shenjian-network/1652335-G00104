#include "mainProc.h"
#include "SPL.h"
#include "snl.h"
#include "sdl.h"

int PROJ_ID;

void forkSender(int & shmid, procState & procS, string (&argvStr)[maxArgc], int procType)
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
			cout << "snl prepare done" << endl;
			snl(pidArr, argvStr);
			exit(0);
		}
		else
		{
			procS = ePL;
			prepareProc(pidArr, procS, shmid, procType);
			cout << "spl prepare done" << endl;
			SPL(pidArr, argvStr, procType);
			exit(0);
		}
	}
	else
	{
		procS = eDL;
		prepareProc(pidArr, procS, shmid, procType);
		cout << "sdl prepare done" << endl;
		sdl(pidArr);
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	int procType = 0;

	PROJ_ID = procType ? 666 : 233;

	srand(time(NULL) + PROJ_ID);

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