#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int main( int argc, char* argv[] ){
	switch (argc) {
		case 0: perror("Something really odd happened!!!"); return -1;
		case 1: perror("Missing source filename"); return -2;
		case 2: perror("Missing destination filename"); return -3;
	}
	int letter;

	errno = 0;
	FILE* f1 = fopen(argv[1], "r");
	if (errno || f1 == NULL) {
		perror("Failed to open source file");
		return errno;
	}

	errno = 0;
	FILE* f2 = fopen(argv[2], "w");
	if (errno || f2 == NULL) {
		perror("Failed to open destination file");
		return errno;
	}

	while ((letter = getc(f1)) != EOF) {
		putc(letter, f2);
	}
	struct stat src_stat;
	stat(argv[1], &src_stat);
	fclose(f2);
	fclose(f1);
	printf("Successfully copied: %s -> %s\n", argv[1], argv[2]);
	errno = 0;
	if (chmod(argv[2], src_stat.st_mode) == 0) {
		puts("Permissions successfully copied");
	} else {
		perror("Failed to copy permissions");
	}
	return 0;
}
