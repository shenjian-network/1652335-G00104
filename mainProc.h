#ifndef MAINPROC_H_
#define MAINPROC_H_

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
using namespace std;

extern char **environ;
static char **g_main_Argv = NULL; /* pointer to argument vector */
static char *g_main_LastArgv = NULL; /* end of argv */
static int PROJ_ID;
enum procState
{
	eNL,
	eDL,
	ePL
};
void workDone(const int shmid, const procState & procS);
void setproctitle_init(int argc, char **argv, char **envp);
void setproctitle(const char *fmt, ...);
static int commShm(int size, int shmflag);
int createShm(int size);
int getShmid();
int destroyShm(int shmid);
void prepareProc(int *pidArr, const procState & procS, const int shmid, bool procType);
void checkArgc(int argc, bool procType);
void createDaemon();

#endif
