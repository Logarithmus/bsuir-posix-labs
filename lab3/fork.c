#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char** argv) {
	pid_t pid = fork();
	if (pid > 0) {
		pid = fork();
	} else if (pid == -1) {
		perror("fork() failed");
		return errno;
	}
	if (pid > 0) {
		system("ps x");
	}

	const int TIME_STR_MAX_SIZE = 20 + 8 + 3 + 1;
	char time_str[TIME_STR_MAX_SIZE];
	struct timespec tspec;
	clockid_t clkid;
	clock_getcpuclockid(0, &clkid);
	clock_gettime(clkid, &tspec);
	int h = tspec.tv_sec / 3600;
	int m = (tspec.tv_sec - h * 3600) / 60;
	int s = tspec.tv_sec - h * 3600 - m * 60;
	int ms = tspec.tv_nsec / 1000000;
	snprintf(time_str, TIME_STR_MAX_SIZE, "%i:%i:%i:%i", h, m, s, ms);
	printf("PID: %i\nParent PID: %i\nTime: %s\n", getpid(), getppid(), time_str);
	while (wait(NULL) > 0);
	return 0;
}
