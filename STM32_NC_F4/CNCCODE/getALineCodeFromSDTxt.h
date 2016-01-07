#ifndef __GETALINE_H
#define __GETALINE_H
#include "malloc.h"
//#include "ff.c"
//char * getALineCode();
char *str;
int tempNum=0;

// comm variables
#define MAX_CMD_SIZE 96
#define BUFSIZE 8
char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];

char * getALineCode(FIL *m_file)
{

	
	mymemset(str,'\0',96);

	if ((f_gets(str,96,m_file)!=NULL))//?????
	{

		tempNum++;
		return str;

	}
	else{
		printf("\r\nFile read success!\n");

		return NULL;
	}

	
}

#endif  //#ifndef __GETALINE_H

