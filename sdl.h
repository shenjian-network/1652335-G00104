#ifndef SDL_H_
#define SDL_H_
#include <iostream>
#include <vector>
#include <queue>
#include <sys/time.h>
#include <algorithm>
#include <time.h>
#include "common.h"
using namespace std;
typedef struct{
    seq_nr k;
    int tout;
}time_node;

const int timeout_interval=100000;
const int timeout_ack_interval=30000;

void sdl(int* pidArr);



#endif // SDL_H_