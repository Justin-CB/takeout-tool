#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "youtube-history-processor.h"
#include "util.h"
#define INPUT_LENGTH 200
#define PROCESSING_LENGTH 400
/* we may need to append more data to the string */
#define DATA_START "<body>"
#define NUM_DIVS 3
/*Each history item is*
 *Contained in 3 div's*/




/* return 1=success *
 * 0=failure or EOF */
static int read_from_file_pointer(int length, char * string, int initializing,...)
{
	static FILE * file;
	va_list arguments;
	if(initializing) {
		va_start(arguments, initializing);
		file = va_arg(arguments, FILE *);
		va_end(arguments);
		return 1;
	}
	/*else*/ if(fgets(string,length,file)) {
		return 1;/*success*/
	}
	/*else*/
		return 0;/*failure or EOF*/
}
static int read_from_file(int length, char * string, int initializing,...)
{
	va_list arguments;
	FILE * file;
	char * path;
	if(initializing) {
		va_start(arguments, initializing);
		path = va_arg(arguments,char *);
		va_end(arguments);
		file=fopen((const char *)path, "r");
		if(file) {
			return read_from_file_pointer(0,NULL,1,file);
		}
		/*else*/
			return 0;
	}
	/*else*/
	return read_from_file_pointer(length,string,0);
}
youtube_item * process_youtube_history_from_file(char * path)
{
	if(!read_from_file(0,NULL,1,path)) {
		fputs("Error opening file\n", stderr);
		exit(1);
	}
	return process_youtube_history((int (*)(int,char *,int))&read_from_file);
}
youtube_item * process_youtube_history_from_file_pointer(FILE * file_pointer)
{
	if(!read_from_file_pointer(0,NULL,1,file_pointer)) {
		fputs("What in tarnation!?!?",stderr);
		exit(3);
	}
	return process_youtube_history((int (*)(int,char *,int))&read_from_file_pointer);
}
static int read_more(int (*read)(int, char *, int), int read_length, char * read_string, int * data_length, char * data_string)
{
	int ret;
	if(strlen(data_string)+read_length>=(*data_length)) {
		(*data_length)+=read_length;
		if(!(data_string=(char *)realloc((void *)data_string,(*data_length)))) {
			fputs("Error allocating mem.\n",stderr);
			exit(1);
		}
	}
	ret=read(read_length,read_string,0);
	strcat(data_string,(const char *)read_string);
	return ret;
}
static int get_tag(const char * tag, int (*read)(int, char *, int), int read_length, char * read_string, int * data_length, char * data_string)
{
	/* We do like this b/c we mightn't've read far enough. *
	 * It's easier to do a "while" loop than to nest the   *
	 * "strstr" in an else, even if that might technically *
	 * be more correct.                                    */
	char * tmp;
	while(!tmp=strstr((const char *)processing_data,tag)) {
		if(!read_more(read,read_length,read_string,data_length,data_string)) {
			return 0;
		}
	}
	while(!tmp=strchr((const char *)processing_data,'>')) {
		if(!read_more(read,read_length,read_string,data_length,data_string)) {
			return 0;
		}
	}
	tmp++;/*shift past '>'*/
	roll_string(data_string,tmp);
	return 1;
}
#define get_div(read,read_length,read_string,data_length,data_string) get_tag("<div",read,read_length,read_string,data_length,data_string)
youtube_item * process_youtube_history(int (*read)(int, char *, int))
{
	char input[INPUT_LENGTH];
	youtube_item * ret=(youtube_item *)malloc(sizeof(youtube_item));
	char * tmp;
	char * processing_data=(char *)malloc(PROCESSING_LENGTH);
	int processing_length=PROCESSING_LENGTH;
	int eof=0;
	var i;
	if(!ret||!processing_data) {
		fputs("Couldn't allocate memory\n",stderr);
		exit(1);
	}
	if(!read(INPUT_LENGTH,input,0)) {
		fputs("Read Error or Premature EOF\n",stderr);
		exit(2);
	}
	while(!(tmp=(strstr((const char *)input,DATA_START)))) {
		/* We keep strlen(DATA_START) characters from the *
		 * previous read in case DATA_START is spread     *
		 * over more than one read.                       */
		roll_string(input,(input+(strlen(input)-strlen(DATA_START))));
		if(!(read((INPUT_LENGTH-strlen(DATA_START)),(input+strlen(DATA_START)),0))) {
			fputs("Error or premature EOF reading data\n",stderr);
			exit(2);
		}
	}
	tmp+=strlen(DATA_START);
	strcpy(processing_data,tmp);
	if(!get_div(read,INPUT_LENGTH,input,&processing_length,processing_data)) {
		fputs("Err or premature EOF reading data\n",stderr);
		exit(2);
	}
	while(1) {
		for(i=0;i<NUM_DIVS;i++) {
			if(!get_div(read,INPUT_LENGTH,input,&processing_length,processing_data)) {
				eof=1;
			}
		}
	}
	return ret;

}
