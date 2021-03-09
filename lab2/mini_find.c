#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>

const char INDENT[] = "│  ";
const char LINE_END_INDENT[] = "├── ";
const char LAST_ITEM_INDENT[] = "└── ";
const size_t INDENT_LEN = sizeof(INDENT) / sizeof(INDENT[0]);
const size_t LINE_END_INDENT_LEN = sizeof(LINE_END_INDENT) / sizeof(LINE_END_INDENT[0]);
const size_t LAST_ITEM_INDENT_LEN = sizeof(LAST_ITEM_INDENT) / sizeof(LAST_ITEM_INDENT[0]);
const char OFF_T_LEN_MAX = 8 * sizeof(off_t) * (M_LN2 / M_LN10) + 2 + 1;
const char DATE_LEN_MAX = 8 * 3 * sizeof(int) * (M_LN2 / M_LN10) + 2 + 1;
const size_t INDENT_SIZE = PATH_MAX * INDENT_LEN + LINE_END_INDENT_LEN + LAST_ITEM_INDENT_LEN + 1;
const size_t NAME_SIZE_DATE_SIZE = FILENAME_MAX + 1 + OFF_T_LEN_MAX + 1 + DATE_LEN_MAX + 1;
const size_t BUF_SIZE = INDENT_SIZE + NAME_SIZE_DATE_SIZE; 
const int MAX_DATE_STR_SIZE = 20 + 1 + 2 + 1 + 2 + 1;

int visited_files_count = 0;

struct cmdopts {
	char* dir;
	FILE* outfile;
	off_t min_size, max_size;
} options = {NULL, NULL, -1, -1};

int parse_cmdopts(char** argv) {
	options.dir = argv[1];

	errno = 0;
	char* leftover = NULL;
	options.min_size = strtol(argv[2], &leftover, 10);
	if (leftover == NULL || *leftover != '\0' || errno != 0) {
		perror("Invalid min size");
		return -1;
	}
	
	errno = 0;
	options.max_size = strtol(argv[3], &leftover, 10);
	if (leftover == NULL || *leftover != '\0' || errno != 0) {
		perror("Invalid max size");
		return -2;
	} else if (options.max_size < options.min_size) {
		perror("Maximum size can't be smaller than minimum size");
		return -21;
	}

	options.outfile = fopen(argv[4], "w");
	return 0;
}	

bool check_file(const struct stat* stat) {
	return	(S_ISREG(stat->st_mode)) && //is regular file
			(stat->st_size >= options.min_size) &&
			(stat->st_size <= options.max_size);
}

bool is_nonempty_dir(const char* dirname) {
	DIR *dir = opendir(dirname);
	if (!dir) { // Not a directory or doesn't exist
		return false;
	}
	int n = 0;
	struct dirent* direntry = NULL;
	while (((direntry = readdir(dir)) != NULL) && (n <= 2)) {
		++n;
	}
	closedir(dir);
	return (n > 2); // . and ..
}

void print_indent(const char* line_end_indent, int count) {
	for (int i = 0; i < count - 1; ++i) {
		fprintf(options.outfile, "%s", INDENT);
	}
	fprintf(options.outfile, "%s", line_end_indent);
}



int print_entry(const char* path, const struct stat* stat,
			   int info, struct FTW* ftw) {
	static int last_level = 0;
	// print empty line after the last file in directory
	if (ftw->level < last_level) {
		print_indent(INDENT, ftw->level);
		fprintf(options.outfile, "\n");
	}
	bool is_non_empty_dir = S_ISDIR(stat->st_mode) && is_nonempty_dir(path);
	bool is_good_file = check_file(stat);
	if (is_non_empty_dir || is_good_file) {
		print_indent(LINE_END_INDENT, ftw->level);
		if (is_good_file) {
			visited_files_count += 1;
			fprintf(options.outfile, "%s\t%s\t%zi\n",
					path, path + ftw->base, stat->st_size);
		} else {
			fprintf(options.outfile, "%s\n", path + ftw->base);
		}
	}
	last_level = ftw->level;
	return 0;
}

int main(int argc, char** argv) {
	switch (argc) {
		case 1: perror("Directory name is missing"); return -1;
		case 2: perror("Min size is missing"); return -2;
		case 3: perror("Max size is missing"); return -3;
		case 4: perror("Output file name is missing"); return -4;
	}
	int code = parse_cmdopts(argv);
	if (code) {
		return code;
	}
	errno = 0;
	int fd_limit = 15;
	int flags = 0;
	if (nftw(options.dir, print_entry, fd_limit, flags) == -1) {
		strerror(errno);
		return -1;
	}
	fclose(options.outfile);
	printf("Total files visited: %i\n", visited_files_count);
	return 0;
}
