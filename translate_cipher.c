#include "csv.h"
#include "stack.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void free_cipher(Cipher* to_be_translated, int cipher_count){
    for (int cipher_index = 0; cipher_index < cipher_count; cipher_index++)
        free(to_be_translated[cipher_index].string);

    free(to_be_translated);
}

int find_col_index(char* col_part, char** col_names, int col_count) {
    int left = 0;
    int right = col_count;
    while (left <= right) {
        
        int mid = left + (right - left) / 2;
        int cmp = strcmp(col_part, col_names[mid]);

        if (cmp == 0) {
            return mid;
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    fprintf(stderr, "I can't find col index.\n");
    return ERROR;
}

int find_row_index(int row_num, int* row_names, int row_count) {
    int left = 0;
    int right = row_count;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (row_names[mid] == row_num) {
            return mid;
        } else if (row_names[mid] < row_num) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    fprintf(stderr, "I can't find row index.\n");
    return ERROR;
}


int find_value(char* cell, char** col_names, int* row_names, int row_count, int col_count, int* table_data, int* is_error) {

    int i = 0;
    while (isalpha(cell[i])) {
        i++;
    }
    
    if (i == 0) {
        char* end_ptr;
        int num = (int)strtol(cell, &end_ptr, 10);
        if (*end_ptr != '\0'){
            *is_error = ERROR;
            return ERROR;
        }
        return num;
    }

    char* column = strdup(cell);
    
    column[i] = '\0';
    
    char* row_part = cell + i;
    if (*row_part == '\0') {
        fprintf(stderr, "Where the name of row?\n");
        *is_error = ERROR;
        return ERROR;
    }

    char* end_ptr;
    int row_num = (int) strtol(row_part, &end_ptr, 10);
    if (*end_ptr != '\0'){
        fprintf(stderr, "Wrong name of row!\n");
        *is_error = ERROR;
        return ERROR;
    }
    
    int col_index = find_col_index(column, col_names, col_count);
    if (col_index == ERROR) {
        *is_error = ERROR;
        return ERROR;
    }
    
    int row_index = find_row_index(row_num, row_names, row_count);
    if (row_index == ERROR) {
        *is_error = ERROR;
        return ERROR;
    }
    
    free(column);
    int index = (row_index - 1) * col_count + (col_index - 1);
    return table_data[index];
}



void operate(Stack* numbers, char op, int* is_error){
    int num1 = pop(numbers, is_error);
    if (*is_error == NOT_ENOUGH){
        return;
    }
    *is_error = SUCCESS;
    int num2 = pop(numbers, is_error);
    if (*is_error == NOT_ENOUGH){
        return;
    }
    
    
    int res;
    switch(op){
        case '+': res = num1 + num2;    break;
        case '-': res = num1 - num2;    break;
        case '*': res = num1 * num2;    break;
        case '/': {
            if (num2 == 0){
                fprintf(stderr, "Division by zero!\n");
                *is_error = ERROR;
                return;
            }
            res = num1 / num2;    
            break;
        }
        default: {
            fprintf(stderr, "Unknown operation %s.\n", &op);
            *is_error = ERROR;
            return;
        }
    }
    if (push(res, numbers) == ERROR){
        *is_error = ERROR;
        return;
    }
}

int calculate_in_cell(char* cell_str, char** col_names, int* row_names, int** table_data, 
                      int row_count, int col_count, int cell_in_cypher) {

    Stack *numbers = init(START_STACK_SIZE);
    if (numbers->massive == NULL)
        return ERROR;
    
    Stack *operations = init(START_STACK_SIZE);
    if (operations->massive == NULL) {
        uninit(numbers);
        return ERROR;
    }

    char* start = cell_str;
    
    for (int i = 0; cell_str[i] != '\0'; i++) {
        char sym = cell_str[i];
        
        if (sym == '*' || sym == '/' || sym == '-' || sym == '+') {
            
            cell_str[i] = '\0';
            int is_error = SUCCESS;
            int value = find_value(start, col_names, row_names, row_count, col_count, *table_data, &is_error);
            if (is_error == ERROR)
                goto cleanup;
            
            if (push(value, numbers) == ERROR)
                goto cleanup;
            

            while (operations->elem_count > 0) {
                int prev_op = peek(operations, &is_error);
                if (is_error == NOT_ENOUGH){
                    goto cleanup;
                }
                if ((prev_op == '*' || prev_op == '/') && (sym == '+' || sym == '-')) {
                    pop(operations, &is_error);
                    if (is_error == NOT_ENOUGH){
                        goto cleanup;
                    }
                    is_error = SUCCESS;
                    operate(numbers, prev_op, &is_error);
                    if (is_error != SUCCESS)
                        goto cleanup;
                } else {
                    break;
                }
            }
            
            if (push(sym, operations) == ERROR)
                goto cleanup;
            
            start = cell_str + i + 1;
        }
    }

    
    int is_error = SUCCESS;
    int value = find_value(start, col_names, row_names, row_count, col_count, *table_data, &is_error);
    if (is_error == ERROR) {
        fprintf(stderr, "Invalid cell reference: %s\n", start);
        goto cleanup;
    }

    if (push(value, numbers) == ERROR)
        goto cleanup;

    while (operations->elem_count > 0) {
        int op = pop(operations, &is_error);
        is_error = SUCCESS;
        operate(numbers, op, &is_error);
        if (is_error != SUCCESS)
            goto cleanup;
    }
    
    if (numbers->elem_count != 1) {
        fprintf(stderr, "Invalid expression\n");
        goto cleanup;
    }
    
    int result = pop(numbers, &is_error);
    
    if (is_error != NOT_ENOUGH)
        (*table_data)[cell_in_cypher] = result;

    uninit(numbers);
    uninit(operations);
    return result;

cleanup:
    uninit(numbers);
    uninit(operations);
    return ERROR;
}

int translate_cipher(Cipher* to_be_translated, int* table_data, int* row_names, char** col_names, 
                     int row_count, int col_count, int cipher_count){

    for (int cipher_index = 0; cipher_index < cipher_count; cipher_index++){
        Cipher cur = to_be_translated[cipher_index];
        char* str = cur.string;
        if (str[0] != '='){
            fprintf(stderr, "Wrong data in table cell. You have %s.\n", str);
            return ERROR;
        }

        char* end_ptr;
        long num = strtol(str + 1, &end_ptr, 10);
        if (*end_ptr == '\0'){
            table_data[cur.cell] = (int)num;
            continue;
        }
        
        int is_error = calculate_in_cell(str + 1, col_names, row_names, &table_data, row_count, col_count, cur.cell);
        if (is_error == ERROR)
            return ERROR;
    }
    return SUCCESS;
}