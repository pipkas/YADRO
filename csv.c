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
    int cipher_count;
    int* table_data = read_lines(file, col_count, &row_names, &to_be_translated, &row_count, &cipher_count);
    
    if (table_data == NULL){
        free(col_names[0]);
        free(col_names);
        return ERROR;
    }
    int is_error = SUCCESS;
    if (cipher_count != 0)
        is_error = translate_cipher(to_be_translated, table_data, row_names, col_names, row_count, col_count, cipher_count);
    if (is_error != ERROR)
        print_table(col_count, row_count, row_names, col_names, table_data);

    
    free_cipher(to_be_translated, cipher_count);
    free(row_names);
    free(table_data);
    free(col_names[0]);
    free(col_names);
    return 0;
}