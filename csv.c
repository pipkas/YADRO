#include "csv.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    print_table(col_count, row_count, row_names,col_names, table_data);
    free(row_names);
    free(to_be_translated);
    free(table_data);
    free(col_names[0]);
    free(col_names);
    return 0;
}