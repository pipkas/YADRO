#pragma once
#include <stdio.h>

static const int ERROR  = -1;
static const int SUCCESS = 0;
static const int NOT_ENOUGH = -2;

typedef struct Cipher{
    char* string;
    int cell;
} Cipher;

int* read_lines(FILE* file, int col_count, int** row_names, Cipher** to_be_translated, int* row_count, int* cipher_count);
int show_csv_file(FILE* file);
void print_table(int col_count, int row_count, int *row_names, char** col_names, int* table_data);
char* read_line(FILE* file, int *size, int *is_error);
char* read_element(char** line);
char** read_col_names(FILE* file , int* col_count);
int translate_cipher(Cipher* to_be_translated, int* table_data, int* row_names, char** col_names, 
    int row_count, int col_count, int cipher_count);

void free_cipher(Cipher* to_be_translated, int cipher_count);
char* my_strchrnul(const char *s, int c);

