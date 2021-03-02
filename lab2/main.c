#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

void listdir(const char *name, int indent) {
	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(name)))
		return;

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			if (!strcmp(entry->d_name, ".") == 0 && !strcmp(entry->d_name, "..") == 0) {
				char path[1024];
				snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
				printf("%*s[%s]\n", indent, "", entry->d_name);
				listdir(path, indent + 2);
			}
		} else {
			char path[1024];
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			struct stat s;
			stat(path, &s);
			printf("%*s %s %l\n", indent, "", entry->d_name, s.st_size);
		}
	}
	closedir(dir);
}

int main(void) {
	listdir("/home/artur/BSUIR", 0);
	return 0;
}
