#include "csv.h"
#include <stdio.h>
#include <stdlib.h>


static void clean_line(char* line, int* line_size) {
    char* end = line + *line_size;
    if (end > line && *(end-1) == '\n' ) {
        *(--end) = '\0';
        (*line_size)--;
    }
    if (end > line && *(end-1) == '\r') {
        *(--end) = '\0';
        (*line_size)--;
    }
}

char* read_line(FILE* file, int *size, int *is_error){
    
    int line_size = 1024;
    char* line = malloc(line_size * sizeof(char));
    if (!line) {
        perror("Error in reallocing line in malloc function.\n");
        *is_error = ERROR;
        return NULL;
    }
    int is_end = NOT_ENOUGH;
    int shift = 0;
    //int iter_num = 1;
    int pos = ftell(file);

    while (is_end != SUCCESS){
        char* line_ptr = fgets(line + shift, line_size - shift, file);
        if (feof(file)){
            free(line);
            *is_error = SUCCESS;
            return NULL;
        }
        if (line_ptr == NULL) {
            perror("Input error in function 'fgets'.\n");
            free(line);
            *is_error = ERROR;
            return NULL;
        }
        int diff = ftell(file) - pos;
        if ((diff == line_size - 1) && (line[diff - 1] != '\n')){
            shift = line_size;
            line_size *= 2;
            line = realloc(line, line_size * sizeof(char));
            if (!line) {
                perror("Error in reallocing line in realloc function.\n");
                *is_error = ERROR;
                return NULL;
            }
            continue;
        }
        is_end = SUCCESS;
    }
    line_size = ftell(file) - pos;
    *size = line_size;
    clean_line(line, &line_size);
    *is_error = SUCCESS;
    return line;
}