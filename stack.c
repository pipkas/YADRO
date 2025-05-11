#include "csv.h"
#include "stack.h"
#include <stdlib.h>


Stack* init(int capacity){
    Stack* stack = malloc(capacity * sizeof(Stack));
    if (!stack) {
        perror("Error in reallocing stack in malloc function.\n");
        stack->massive = NULL;
        return stack;
    }
    stack->massive = malloc(capacity * sizeof(int));
    if (!stack->massive) {
        perror("Error in reallocing stack->massive in malloc function.\n");
        stack->massive = NULL;
        return stack;
    }
    stack->capacity = capacity;
    stack->elem_count = 0;
    return stack;
}

void uninit(Stack* stack){
    free(stack->massive);
    free(stack);
}

int push(int elem, Stack* stack){
    if (stack->capacity < stack->elem_count + 1){
        stack->massive = realloc(stack->massive, stack->capacity * 2 * sizeof(int));
        if (!stack->massive) {
            perror("Error in reallocing stack.massive in realloc function.\n");
            return ERROR;
        }
        stack->capacity *= 2;
    }
    stack->massive[stack->elem_count] = elem;
    stack->elem_count++;
    return SUCCESS;
}

int pop(Stack* stack, int *is_error){
    int elem = peek(stack, is_error);
    if (*is_error != NOT_ENOUGH)
        --stack->elem_count;

    return elem;
}

int peek(Stack* stack, int *is_error){
    if (stack->elem_count == 0){
        printf("No elements in stack when you use pop function.\n");
        *is_error = NOT_ENOUGH;
        return NOT_ENOUGH;
    }
    
    return stack->massive[stack->elem_count - 1];
}