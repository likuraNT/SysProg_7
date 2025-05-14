#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void file_searching(const char* path, const char* word) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror(path);
        return;
    }

    char* line = NULL;
    size_t len = 0;
    int line_num = 1;

    while (getline(&line, &len, file) != -1) {
        if (strstr(line, word)) {
            printf("%s:%d: %s", path, line_num, line);
        }
        ++line_num;
    }

    free(line);
    fclose(file);
}

void directory_searching(const char* path, const char* word) {
    DIR* dir = opendir(path);
    if (!dir) {
        perror(path);
        return;
    }

    struct dirent* entry;
    char full_path[PATH_MAX];

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) {
            perror(full_path);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            directory_searching(full_path, word);
        } else if (S_ISREG(st.st_mode)) {
            file_searching(full_path, word);
        }
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    char* _dir = NULL;
    char* word = NULL;
    int rez = 0;

    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: %s [directory] <search_word>\n", argv[0]);
        return EXIT_FAILURE;
    } else if (argc == 4) {
        _dir = argv[2];
        word = argv[3];
        printf("%s %s %s %s\n", argv[0], argv[1] , argv[2], argv[3]);
    } else if (argc == 3 && strcmp(argv[1], "-i") != 0) {
        _dir = argv[1]; 
        word = argv[2];
    } else if (argc == 3 && strcmp (argv[1], "-i") == 0) {
        _dir = strcat(getenv("HOME"), "/files");
        word = argv[2];
    } else {
        _dir = strcat(getenv("HOME"), "/files");
        word = argv[1];
    }

	while ( (rez = getopt(argc, argv, "i")) != -1){
		switch (rez) {
		case 'i': 
            printf("found argument \"i\".\n");
            if (argc == 4) {
                for (int i = 0; i < strlen(argv[3]); ++i)
                {
                    argv[3][i] = tolower(argv[3][i]);
                }
            } else if (argc == 3) {
                for (int i = 0; i < strlen(argv[2]); ++i)
                {
                    argv[2][i] = tolower(argv[2][i]);
                }
            } else {
                for (int i = 0; i < strlen(argv[1]); ++i)
                {
                    argv[1][i] = tolower(argv[1][i]);
                }
            }
            break;
		default: 
            printf("found no arguments\n");
            break;
		} 
	}

    directory_searching(_dir, word);
    return 0;
}