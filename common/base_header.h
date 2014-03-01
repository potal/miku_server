#pragma once
					
#ifdef _WIN32

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <process.h>
#include <iostream>
using namespace std;

#else

#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <stdio.h>
#include <string.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <sys/select.h> 
#include <time.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
typedef   int   SOCKET;

typedef unsigned long DWORD;

typedef struct sockaddr_in SOCKADDR_IN;


#endif	
