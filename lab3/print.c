#include <stdio.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		perror("Message is missing");
		return -1;
	}
	printf("%s\n", argv[1]);
	return 0;
}
