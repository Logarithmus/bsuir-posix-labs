#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[], char** env) {
	switch (argc) {
		case 0: perror("Something really odd happened!!!"); return -1;
		case 1: perror("Missing directory name"); return -2;
	}

	DIR* dir = opendir(getenv("PWD"));
	errno = 0;
	if (errno != 0 || !dir){
		perror("Failed to open current directory");
		return errno;
	}

	puts("Current directory:");
	struct dirent* dir1;
	while ((dir1 = readdir(dir))) {
		printf("%s\n",dir1->d_name);
	}
	
	puts("\nSelected directory:");
	errno = 0;
	DIR* dir2 = opendir(argv[1]);
	if (!dir2){
		perror("Failed to open selected directory");
		return errno;
	}

	struct dirent* dir3;
	while ((dir3 = readdir(dir2))) {
		printf("%s\n", dir3->d_name);
	}
	closedir(dir2);
	return 0;
}
