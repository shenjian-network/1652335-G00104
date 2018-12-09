#ifndef SNL_H_
#define SNL_H_
#include "common.h"
#include "nl.h"
// 主函数(fork) 输入为sdl_pid
void snl(int* pidArr, std::string (&argvStr)[maxArgc]);

#endif // SNL_H_