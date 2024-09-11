#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void count_brackets_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    int open_count = 0;
    if (!file) {
        perror("Не удаётся открыть файл.");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '{' || ch == '(' || ch == '[') {
            open_count++;
        } else if (ch == '}' || ch == ')' || ch == ']') {
            open_count--;
        }
	   if (open_count < 0) break;
    }

    fclose(file);
    
    //if (open_count < 0) open_count *= -1; 
    printf("Файл: %s | Скобок: %d ", filename, open_count);
    if (open_count != 0) printf("В файле лишние скобки!\n");
    else printf("Лишних скобок нет.\n");
    exit(EXIT_SUCCESS);
	
}

void count_brackets_in_directory(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);
    
    if (dp == NULL) {
        perror("Нет доступа к каталогу.");
        return;
    }

    int total_open = 0;

    while ((entry = readdir(dp)) != NULL) {
        // Проверяем, является ли файл .c или .h
        if ((strstr(entry->d_name, ".c") != NULL) || strstr(entry->d_name, ".h") != NULL) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);
            
	    pid_t pid = fork(); // Создаем дочерний процесс
            
            if (pid < 0) {
                perror("Fork failed");
                continue;
            }
            
            if (pid == 0) {
                // Дочерний процесс
                count_brackets_in_file(filepath);
            } else {
                // Родительский процесс
                wait(NULL); // Ждем завершения дочернего процесса
            }
        }
    }

    closedir(dp);
   
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Расположение: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    count_brackets_in_directory(argv[1]);
    
    return EXIT_SUCCESS;
}
