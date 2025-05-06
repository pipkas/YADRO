#include "csv.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int check_col_name(char* col_name){
    for (int name_pos = 0; col_name[name_pos] != '\0'; name_pos++){
        int tolower_c = tolower(col_name[name_pos]);
        if (tolower_c < 'a' || tolower_c > 'z'){
            fprintf(stderr, "Wrong format of the csv file, you have incorrect sumbols in columns names.\n");
            return ERROR;
        }
    }
    return SUCCESS;
}

int show_csv_file(FILE* file){
    int col_count;
    char** col_names = read_col_names(file, &col_count);
    if (col_names == NULL)
        return ERROR;
    
    int* row_names = NULL;
    Cipher* to_be_translated = NULL;
    int row_count;
    int* table_data = read_lines(file, col_count, &row_names, &to_be_translated, &row_count);
    if (table_data == NULL){
        free(col_names[0]);
        free(col_names);
        return ERROR;
    }
    for (int i = 0; i <= col_count; i++)
    printf("%s\n", col_names[i]);
    
    free(row_names);
    free(to_be_translated);
    free(table_data);
    free(col_names[0]);
    free(col_names);
    return 0;
}