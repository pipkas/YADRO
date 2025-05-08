#define _GNU_SOURCE
#include "csv.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


static void ptr_swap(char** line_ptr, char** sym_ptr) {
    char* save_ptr = *sym_ptr;
    *sym_ptr = *line_ptr;
    *line_ptr = save_ptr;
}


static int check_col_name(char* col_name, char* prev_col_name){
    if (col_name == NULL)
        return ERROR;

    for (int name_pos = 0; col_name[name_pos] != '\0'; name_pos++){
        int tolower_c = tolower(col_name[name_pos]);
        if (tolower_c < 'a' || tolower_c > 'z'){
            fprintf(stderr, "Wrong format of the csv file, you have incorrect simbols in columns names.\n");
            return ERROR;
        }
    }

    if (prev_col_name == NULL)
        return SUCCESS;
    
    if (strcmp(col_name, prev_col_name) <= 0){

        fprintf(stderr, "Wrong format in csv file. %s <= %s - column names must be in ascending order.\n", 
                col_name,  prev_col_name);

        return ERROR;
    }
        
    
    return SUCCESS;
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
    col_names[0] = line;

    char* found_sym = strchrnul(line, ',');
    while (found_sym != NULL){
        *found_sym = '\0';
        found_sym++;
        ptr_swap(&found_sym, &line);
        int is_error = SUCCESS;
        if (col_index != 0)
            is_error = check_col_name(found_sym, col_names[col_index - 1]);
        if (is_error == ERROR){
            free(col_names[0]);
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