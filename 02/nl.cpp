#include "nl.h"
void setMask(int sigv,void(*si)(int x))
{
    struct sigaction newact, oldact;
	newact.sa_handler = si;
	sigemptyset(&newact.sa_mask);
	sigaddset(&newact.sa_mask, SIG_DISABLE_NETWORK_LAYER);
    sigaddset(&newact.sa_mask, SIG_ENABLE_NETWORK_LAYER);
    sigaddset(&newact.sa_mask, SIG_ALL_START);
    sigaddset(&newact.sa_mask, SIG_NETWORK_READ);
	newact.sa_flags = 0;
    sigaction(sigv, &newact, &oldact);
}
