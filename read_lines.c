#include "csv.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



static int fill_row(int* table_values, int col_count, char* line, int* row_names, Cipher* to_be_translated, 
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
            cur.string = strdup(element);   //we have to free memory!!!!
            cur.cell = num_elem + line_index * col_count;
            to_be_translated[*cipher_index] = cur;
            (*cipher_index)++;
            continue;
        }

        if (num_elem == 0){
            if (line_index != 0 && row_names[line_index - 1] >= (int)number){

                fprintf(stderr, "Wrong format in csv file. %d >= %d - row names must be in ascending order.\n",
                        row_names[line_index - 1], (int)number);
                return ERROR;
            }
            row_names[line_index] = (int)number;
            continue;
        }
        table_values[(num_elem - 1) + (line_index - 1) * col_count] = (int)number;   
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


int* read_lines(FILE* file, int col_count, int** row_names, Cipher** to_be_translated, int* row_count, int* cipher_count){
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
                free_cipher(*to_be_translated, *cipher_count);
                return NULL;
            }
        }
        
        is_error = fill_row(table_values, col_count, line, *row_names, *to_be_translated, &cipher_index, line_index);
        if (is_error == ERROR){
            free(*row_names);
            free(line);
            free(table_values);
            free_cipher(*to_be_translated, *cipher_count);
            return NULL;
        }
        char* line_end = line + line_size - 1;
        if (read_element(&line_end) != NULL){
            fprintf(stderr, "Too many elements in row.\n");
            free_cipher(*to_be_translated, *cipher_count);
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
        free_cipher(*to_be_translated, *cipher_count);
        return NULL;
    }
    *cipher_count = cipher_index;
    *row_count = line_index;
    return table_values;
}


