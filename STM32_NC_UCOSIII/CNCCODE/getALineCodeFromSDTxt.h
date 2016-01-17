#ifndef __GETALINE_H
#define __GETALINE_H
//#include "malloc.h"
//#include "ff.c"
#include "NCSupport.h"

extern char *str;
extern int tempNum;
// comm variables
#define MAX_CMD_SIZE 96
#define BUFSIZE 8
extern char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];

char * getALineCode(FIL *m_file);

#endif  //#ifndef __GETALINE_H
