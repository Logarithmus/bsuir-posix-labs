#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

void* start_routine(void* arg) {
	const int TIME_STR_MAX_SIZE = 20 + 8 + 3 + 1;
	char time_str[TIME_STR_MAX_SIZE];
	pthread_t tid = pthread_self();
	struct timespec tspec;
	clockid_t clkid;
	pthread_getcpuclockid(tid, &clkid);
	clock_gettime(clkid, &tspec);
	int h = tspec.tv_sec / 3600;
	int m = (tspec.tv_sec - h * 3600) / 60;
	int s = tspec.tv_sec - h * 3600 - m * 60;
	int ms = tspec.tv_nsec / 1000000;
	snprintf(time_str, TIME_STR_MAX_SIZE, "%i:%i:%i:%i", h, m, s, ms);
	printf("TID: %lu\nPID: %i\nTime: %s\n\n", (unsigned long) tid, getppid(), time_str);
	return NULL;
}

int main(int argc, char** argv) {
	start_routine(NULL);

	pthread_t thread1;
	int code = pthread_create(&thread1, NULL, start_routine, NULL);
	if (code) {
		fprintf(stderr, "%s", strerror(code));
		return code;
	}

	pthread_t thread2;
	code = pthread_create(&thread2, NULL, start_routine, NULL);
	if (code) {
		fprintf(stderr, "%s", strerror(code));
		return code;
	}

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return 0;
}
