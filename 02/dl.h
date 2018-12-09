#ifndef DL_H_
#define DL_H_

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
const int timeout_ack_interval=500;


void start_ack_timer();

void stop_ack_timer();

void start_timer(seq_nr k);

void stop_timer(seq_nr k);

void wait_for_event(event_type* event);

void SDL_init_signaction();

#endif // DL_H_