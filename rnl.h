#ifndef RNL_H_
#define RNL_H_
#include "common.h"

// 主函数(fork) 输入为rdl_pid
void rnl(int* pidArr, std::string (&argvStr)[maxArgc]);

#endif // RNL_H_