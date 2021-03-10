#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

typedef struct {
	double x;
	unsigned term_index;
	double term;
} TaylorData;

double factorial(unsigned x) {
	double result = 1;
	for (unsigned i = 2; i <= x; ++i) {
		result *= i;
	}
	return result;
}

void* compute_term(void* raw_data) {
	TaylorData* data = (TaylorData*) raw_data;
	double x = data->x;
	int index = data->term_index;
	int sign = (index / 2) % 2 == 0 ? 1 : -1;
	printf("x = %lf\nindex = %u\npow = %lf\nfactorial = %lf\n\n", x, index, pow(x, index), factorial(index));
	data->term = sign * pow(x, index) / factorial(index);
	printf("TID: %lu\nTerm index: %u\nTerm value: %lf\n\n",
			(unsigned long) pthread_self(), index, data->term);
	return NULL;
}

double sin_taylor(double x, int terms_count) {
	double result = 0;
	TaylorData data[terms_count];
	pthread_t tid[terms_count];
	for (int j = 0; j < terms_count; ++j) {
		data[j].x = x;
		data[j].term_index = 2 * j + 1;
		pthread_create(tid + j, NULL, compute_term, &data[j]);
	}
	for (int j = 0; j < terms_count; ++j) {
		pthread_join(tid[j], NULL);
		result += data[j].term;
	}
	return result;
}

long strtol_checked(const char* str, const char* err_msg) {
	errno = 0;
	long result = strtol(str, NULL, 10);
	if (errno) {
		perror(err_msg);
		exit(errno);
	}
	return result;
}

unsigned long strtoul_checked(const char* str, const char* err_msg) {
	errno = 0;
	unsigned long result = strtoul(str, NULL, 10);
	if (errno) {
		perror(err_msg);
		exit(errno);
	}
	return result;
}

int main(int argc, char** argv) {
	switch (argc) {
		case 0: perror("Something nasty happened!!!"); return -1;
		case 1: perror("Missing K"); return -2;
		case 2: perror("Missing N"); return -3;
		case 3: perror("Missing Taylor series terms count"); return -4;
		case 4: perror("Missing results filename"); return -5;
	}
	
	unsigned long K = strtoul_checked(argv[1], "Wrong K value");
	long N = strtol_checked(argv[2], "Wrong N value");
	unsigned long terms_count = strtoul_checked(argv[3], "Wrong Taylor series terms count");

	double results[K];
	for (unsigned i = 0; i < K; ++i) {
		printf("Computing y[%u]...\n\n", i);
		results[i] = sin_taylor(2 * M_PI * i / N, terms_count);
		printf("y[%u] = %lf\n------------------\n\n", i, results[i]);
	}
	FILE* outfile = fopen(argv[4], "w");
	for (unsigned i = 0; i < K; ++i) {
		printf("y[%u] = %lf\n", i, results[i]);
		fprintf(outfile, "y[%u] = %lf\n", i, results[i]);
	}
	return 0;
}
