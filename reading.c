#define _GNU_SOURCE
#include "csv.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void clean_line(char* line, int* line_size) {
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
    int iter_num = 1;
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

void ptr_swap(char** line_ptr, char** sym_ptr) {
    char* save_ptr = *sym_ptr;
    *sym_ptr = *line_ptr;
    *line_ptr = save_ptr;
}

char* read_element(char** line){
    if (*line == NULL || **line == '\0')
        return NULL;

    char* found_sym = strchrnul(*line, ',');
    *found_sym = '\0';
    char* result_ptr = *line;
    *line = found_sym + 1;
    return result_ptr;
}


int fill_row(int* table_values, int col_count, char* line, int* row_names, Cipher* to_be_translated, 
    int* cipher_index, int line_index){

    for (int num_elem = 0; num_elem <= col_count; num_elem++){
        char* element = read_element(&line);
        if (element == NULL){
            fprintf(stderr, "Not enough elements in row.\n");
            return ERROR;
        }
        char* end_ptr;
        long number = strtol(element, &end_ptr, 10);
        if (*end_ptr != '\0'){
            if (num_elem == 0){
                fprintf(stderr, "Wrong name of row, you use %c, change it to the number.\n", *end_ptr); 
                perror("Error with strtol in red_lines function.\n");
                return ERROR;
            }
            
            Cipher cur;
            cur.string = strdup(element);
            cur.cell = num_elem + line_index * col_count;
            to_be_translated[*cipher_index] = cur;
            (*cipher_index)++;
            continue;
        }

        if (num_elem == 0){
            row_names[line_index] = (int)number;
            //printf("%ld\n", number);
            continue;
        }
        //printf("%ld\n", number);
        table_values[(num_elem - 1) + line_index * col_count] = (int)number;   
    }
    return SUCCESS;
}

static int reallocing_memory(int* table_values, int* row_names, Cipher* to_be_translated, int line_count, int col_count){
    table_values = realloc(table_values, line_count * col_count * sizeof(int));
    if (!table_values) {
        perror("Error in reallocing table_values in realloc function.\n");
        return ERROR;
    }
    row_names = realloc(row_names, line_count * sizeof(int));
    if (!row_names) {
        perror("Error in reallocing row_names in realloc function.\n");
        return ERROR;
    }
    to_be_translated = realloc(to_be_translated, line_count * col_count * sizeof(Cipher));
    if (!to_be_translated) {
        perror("Error in reallocing to_be_translated in realloc function.\n");
        return ERROR;
    }
    return SUCCESS;
}


int* read_lines(FILE* file, int col_count, int** row_names, Cipher** to_be_translated, int* row_count){
    int line_num = 1024;
    int* table_values = malloc(col_count * line_num * sizeof(int));
    if (!table_values) {
        perror("Error in reallocing table_values in malloc function.\n");
        return NULL;
    }
    *row_names = malloc(line_num * sizeof(int));
    if (!*row_names){
        free(table_values);
        perror("Error in reallocing row_names in malloc function.\n");
        return NULL;
    }
    *to_be_translated = malloc(col_count * line_num * sizeof(Cipher));
    if (!*to_be_translated) {
        perror("Error in reallocing to_be_translated in malloc function.\n");
        free(*row_names);
        free(table_values);
        return NULL;
    }

    int is_error;
    int cipher_index = 0;
    int line_index = 0;
    int line_size;
    char* line;
    while((line = read_line(file, &line_size, &is_error))){
            
        line_index++;
        if (line_index > line_num){
            line_num *= 2;
            is_error = reallocing_memory(table_values, *row_names, *to_be_translated, line_num, col_count);
            if (is_error == ERROR){
                free(*row_names);
                free(line);
                free(table_values);
                free(*to_be_translated);
                return NULL;
            }
        }
        
        char* end_ptr;
        is_error = fill_row(table_values, col_count, line, *row_names, *to_be_translated, &cipher_index, line_index);
        if (is_error == ERROR){
            free(*row_names);
            free(line);
            free(table_values);
            free(*to_be_translated);
            return NULL;
        }
        char* line_end = line + line_size - 1;
        if (read_element(&line_end) != NULL){
            fprintf(stderr, "Too many elements in row.\n");
            free(*to_be_translated);
            free(*row_names);
            free(table_values);
            free(line);
            return NULL;
        }
        
        free(line);
    }
    if (is_error == ERROR){
        free(*row_names);
        free(line);
        free(table_values);
        free(*to_be_translated);
        return NULL;
    }

    *row_count = line_index;
    return table_values;
}


char** read_col_names(FILE* file , int* col_count){

    fseek(file, 0, SEEK_SET);
    int line_size;
    int is_error;
    char* line = read_line(file, &line_size, &is_error);
    if (line == NULL)
        return NULL;

    int col_index = 0;

    if (line[0] != ','){
        fprintf(stderr, "Wrong input data. You have to leave empty cell in the upper left corner.\n");
        free(line);
        return NULL;
    }

    char** col_names = malloc(line_size * sizeof(char*));
    if (!col_names) {
        perror("Error in reallocing col_names in malloc function.\n");
        free(line);
        return NULL;
    }
    col_names[0] = line;    //we have to free memory after all

    char* found_sym = strchrnul(line, ',');
    while (found_sym != NULL){
        *found_sym = '\0';
        found_sym++;
        ptr_swap(&found_sym, &line);
        int is_error = check_col_name(found_sym);
        if (is_error == ERROR){
            free(line);
            free(col_names);
            return NULL;
        }
        col_names[col_index] = found_sym;
        col_index++;
        found_sym = strchr(line, ',');
    }
    col_names[col_index] = line;

    *col_count = col_index;
    return col_names;
}
