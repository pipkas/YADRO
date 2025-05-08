#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "csv.h"

int main(int argc, char* argv[]){
    
    if (argc != 2){
        fprintf(stderr, "Wrong number of input elements! \nYou have to write a csv file name.\n");
        return ERROR;
    }
    char* file_name = argv[1];
    FILE* file = fopen(file_name, "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", file_name);
        perror("Error in function fopen");
        return ERROR;
    }
    
    int is_error = show_csv_file(file);
    
    if (is_error == ERROR){
        fclose(file);
        return ERROR;
    }

    fclose(file);
    return 0;
}