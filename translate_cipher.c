#include "csv.h"
#include <stdlib.h>

void free_cipher(Cipher* to_be_translated, int cipher_count){
    for (int cipher_index = 0; cipher_index < cipher_count; cipher_index++)
        free(to_be_translated[cipher_index].string);

    free(to_be_translated);
}

// int find_cell(char* col_name, int row_name, char** col_names, int* row_names){
    
// }

// int translate_cipher(Cipher* to_be_translated, int* table_data, int* row_names, char** col_names, 
//                      int row_count, int col_count, int cipher_count){

//     for (int cipher_index = 0; cipher_index < cipher_count; cipher_index++){
//         Cipher cur = to_be_translated[cipher_index];
//         char* str = cur.string;
//         if (str[0] != '='){
//             fprintf(stderr, "Wrong data in table cell. You have %s.\n", str);
//             return ERROR;
//         }

//         char* end_ptr;
//         int num = strtol(str + 1, &end_ptr, 10);
//         if (end_ptr == '\0'){
//             table_data[cur.cell] = num;
//             continue;
//         }


//     }

// }