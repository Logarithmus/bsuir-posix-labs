#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		perror("Output filename is missing");
		return -1;
	}
	errno = 0;
	FILE* outfile = fopen(argv[1], "w");
	if (errno) {
		perror("Failed to open outfile");
		return errno;
	}

	char c = '\0';
	while (c != 6) {
		errno = 0;
		c = fgetc(stdin);
		if (errno == EBADF) {
			strerror(errno);
			fclose(outfile);
			return errno;
		}
		if (c == 6) {
			puts("^F detected, exiting...");
			fclose(outfile);
			return 0;
		}
		fputc(c, outfile);
		if (errno) {
			perror("Failed to write to file");
			fclose(outfile);
			return errno;
		}
	}
	fclose(outfile);
	return 0;
}
