#pragma once
#include <stdio.h>

static const int ERROR  = -1;
static const int SUCCESS = 0;
static const int NOT_ENOUGH = 1;

typedef struct Cipher{
    char* string;
    int cell;
} Cipher;

char** read_col_names(FILE* file , int* col_count);
int* read_lines(FILE* file, int col_count, int** row_names, Cipher** to_be_translated, int *row_count);
int show_csv_file(FILE* file);
int check_col_name(char* col_name);
void print_table(int col_count, int row_count, int *row_names, char** col_names, int* table_data);

