#pragma once

static const int START_STACK_SIZE = 64; 
typedef struct Stack{
    int* massive;
    int capacity;
    int elem_count;
}Stack;

int peek(Stack* stack, int *is_error);
int pop(Stack* stack, int *is_error);
int push(int elem, Stack* stack);
void uninit(Stack* stack);
Stack* init(int capacity);