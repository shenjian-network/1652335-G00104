#ifndef PL_H_
#define PL_H_

#include <bits/stdc++.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include "common.h"
using namespace std;
const int MAX_COUNT = 10000;
const int MAX_FRAME_SIZE = 1036;
const int ACK_SIZE = 12;
const int NAK_SIZE = 12;
static int socketFd;
static int procType;
void preparePLData();
void PL_receive_SIG_D2P(int, siginfo_t*, void*);
void PL_init_signaction();
#endif