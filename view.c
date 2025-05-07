#include <stdio.h>

void print_table(int col_count, int row_count, int *row_names, char** col_names, int* table_data){
    for (int i = 0; i <= col_count; i++){
        printf("%s", col_names[i]);
        if (i != col_count)
            printf(", ");
    }
    printf("\n");
    for (int line = 1; line <= row_count; line++){
        printf("%d, ", row_names[line]);
        for (int elem_num = 1; elem_num <= col_count; elem_num++){
            printf("%d", table_data[(line - 1) * col_count + (elem_num - 1)]);
            if (elem_num != col_count)
                printf(", ");
        }
        printf("\n");
    }
}