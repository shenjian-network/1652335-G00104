#ifndef NL_H_
#define NL_H_

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
void setMask(int sigv,void(*si)(int x));
#endif