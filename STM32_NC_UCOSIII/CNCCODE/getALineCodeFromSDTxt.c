#include "getAlineCodeFromSDtxt.h"


#include "malloc.h"	 
#include "usart.h"

char *str;
int tempNum=0;

char * getALineCode(FIL *m_file)
{
	mymemset(str,'\0',96);

	if ((f_gets(str,96,m_file)!=NULL))//?????
	{

		tempNum++;
		return str;

	}
	else{
		printf("\r\nFile read over!\n");

		return NULL;
	}

	
}
