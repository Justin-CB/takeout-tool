#include <string.h>
#include <stdlib.h>
void roll_string(char * string, char * roll_to)
{
	char * temp=(char *)malloc(strlen(roll_to)+1);
	strcpy(temp,roll_to);
	strcpy(string,temp);
	free(temp);
}
