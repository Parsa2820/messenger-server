#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/ip.h> 
#include <time.h>
#include "JSONc.c"
#include "fileManage.c"
#define MAX 500
#define PORT 12345
#define SA struct sockaddr 
#define MAXUSERS 100