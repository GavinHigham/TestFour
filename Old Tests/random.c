#include <sys/time.h>

struct timeval tv;   

void init_random() {
	//This is just for getting random numbers.                                                              
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
}