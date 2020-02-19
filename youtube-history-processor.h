#include <time.h>
#include <stdbool.h>
#include <stdio.h>
typedef enum action {
	search,
	watch
} action;
typedef struct youtube_item {
	char * heading;
	action action_type;
	char * action_target;
	char * action_link;
	char * channel;
	char * channel_link;
	time_t activity_time;
	char * products;
	bool last_item;
} youtube_item;
youtube_item * process_youtube_history(int (*read)(int, char *, int));
/*read puts a null-terminated string of length up to the 1st int argument*
 *process_youtube_history passes 0 to the 2nd int arg(3rd arg overall)   *
 *read must return 1 on success & 0 on error or End of Data              */
youtube_item * process_youtube_history_from_file(char * path);
youtube_item * process_youtube_history_from_file_pointer(FILE * file_pointer);
