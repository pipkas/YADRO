#include "csv.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* read_line(FILE* file, int *size){
    
    int line_size = 1024;
    char* line = malloc(line_size * sizeof(char));
    if (!line) {
        perror("Error in reallocing line in malloc function.\n");
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
            return NULL;
        }
        if (line_ptr == NULL) {
            perror("Input error in function 'fgets'.\n");
            free(line);
            return NULL;
        }
        int diff = ftell(file) - pos;
        if ((diff == line_size - 1) && (line[diff - 1] != '\n')){
            shift = line_size;
            line_size *= 2;
            line = realloc(line, line_size * sizeof(char));
            if (!line) {
                perror("Error in reallocing line in realloc function.\n");
                return NULL;
            }
            continue;
        }
        is_end = SUCCESS;
    }
    line_size *= iter_num;
    *size = line_size;

    return line;
}

void ptr_swap(char** line_ptr, char** sym_ptr) {
    char* save_ptr = *sym_ptr;
    *sym_ptr = *line_ptr;
    *line_ptr = save_ptr;
}

char* read_element(char* line){
    char* found_sym = strchr(line, ',');
    *found_sym = '\0';
    found_sym++;
    ptr_swap(&found_sym, &line);
    return found_sym;
}


int* read_lines(FILE* file, int col_count, int** row_names, Cipher** to_be_translated){
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
    *to_be_translated = malloc(col_count * line_num *sizeof(Cipher));
    if (!*to_be_translated) {
        perror("Error in reallocing to_be_translated in malloc function.\n");
        free(*row_names);
        free(table_values);
        return NULL;
    }


    int cipher_index = 0;
    int line_index = 0;
    while(!feof(file)){

        int line_size;
        char* line = read_line(file, &line_size);
        line_index++;
        if (line_index >= line_num){
            line_num *= 2;
            table_values = realloc(table_values, col_count * line_num * sizeof(int));
            if (!table_values) {
                perror("Error in reallocing table_values in realloc function.\n");
                free(table_values);
                free(*to_be_translated);
                return NULL;
            }
            *row_names = realloc(table_values, col_count * line_num * sizeof(int));
            if (!*row_names) {
                perror("Error in reallocing row_names in realloc function.\n");
                free(table_values);
                free(*to_be_translated);
                return NULL;
            }
            *to_be_translated = realloc(*to_be_translated, col_count * line_num * sizeof(Cipher));
            if (!*to_be_translated) {
                perror("Error in reallocing to_be_translated in realloc function.\n");
                free(*row_names);
                free(table_values);
                free(*to_be_translated);
                return NULL;
            }
        }


        char* end_ptr;
        
        for (int num_elem = 0; num_elem <= col_count; num_elem++){
            char* element = read_element(line);
            if (element == NULL){
                fprintf(stderr, "Not enough elements in row.\n");
                free(*to_be_translated);
                free(*row_names);
                free(table_values);
                return NULL;
            }
            long number = strtol(element, &end_ptr, 10);

            if (num_elem == 0 && *end_ptr != '\0'){
                fprintf(stderr, "Wrong name of row, you use %c, change it to the number.\n", *end_ptr); 
                perror("Error with strtol in red_lines function.\n");
                free(*to_be_translated);
                free(*row_names);
                free(table_values);
                return NULL;
            }

            if (num_elem == 0){
                *row_names[num_elem] = number;
                continue;
            }
            
            if (*end_ptr != '\0'){
                Cipher cur;
                cur.string = element;
                cur.cell = num_elem + line_index * col_count;
                *to_be_translated[cipher_index] = cur;
                cipher_index++;
                continue;
            }
            table_values[(num_elem - 1) + line_index * col_count] = (int)number;
            
        }
        if (read_element(line) != NULL){
            fprintf(stderr, "Too many elements in row.\n");
            free(*to_be_translated);
            free(*row_names);
            free(table_values);
            return NULL;
        }

        free(line);
    }

    
    row_names = malloc(line_num * sizeof(int));
    return table_values;
}


char** read_col_names(FILE* file , int* col_count){

    fseek(file, 0, SEEK_SET);
    int line_size;
    char* line = read_line(file, &line_size);
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

    char* found_sym = strchr(line, ',');
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
