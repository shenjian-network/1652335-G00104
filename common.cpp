#include "common.h"

void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void set_lock(int fd, int type)
{
    struct flock old_lock, lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = type;

    fcntl(fd, F_GETLK, &lock);

    /*
	if (lock.l_type != F_UNLCK) {  
        if (lock.l_type == F_RDLCK) 
            printf("Read lock already set by %d\n", lock.l_pid);  
        else if (lock.l_type == F_WRLCK)   
            printf("Write lock already set by %d\n", lock.l_pid);  
	} */

    lock.l_type = type;

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fuck");
        exit(EXIT_FAILURE);
    }

    /*
	switch(lock.l_type) {  
        case F_RDLCK:  
            printf("Read lock set by %d\n", getpid());  
            break;  
  
        case F_WRLCK:  
            printf("Write lock set by %d\n", getpid());  
            break;  
  
        case F_UNLCK:  
            printf("Release lock by %d\n", getpid());  
            break;
	default:  
            break;  
	}*/
}

// 读入数据,如果文件结束则返回0，否则会循环读到size为止
int myRead(int fd, void *buffer, int size)
{
    int retSize=0;
    int RecvSize;
    char *cbuffer=(char*)buffer;
    while (1)
    {
        RecvSize = read(fd, cbuffer, size);
        if (RecvSize < 0)
        {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                continue;
            }
            else{
                die("read error");
            }
        }
        else if(RecvSize==0)
        {
            return 0;
        }
        size-=RecvSize;
        cbuffer+=RecvSize;
        retSize+=RecvSize;
        if(size==0)
            break;
    }
    return retSize;
}

// 写数据
int myWrite(int fd, void *buffer, int size)
{
    int befSize=size;
    int SendSize; 
    char* cbuffer=(char*)buffer;
    while (1)
    {
        SendSize=write(fd, cbuffer, size);
        if (SendSize <= 0)
        {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                continue;
            }
            else
                die("write error");
        }
        size-=SendSize;
        cbuffer+=SendSize;
        if(size==0)
            break;
    }
    return befSize;
}