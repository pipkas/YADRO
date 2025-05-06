PROGRAM = prog

CC = gcc

CFLAGS = -Wall -Wextra -pedantic

LDFLAGS = -L. -lstatic_lib

SRCS = $(wildcard *.c) 

OBJS = $(SRCS:.c=.o)

$(PROGRAM): $(OBJS)  
	$(CC) $(OBJS) -o $(PROGRAM)  

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@  

clean:
	rm -f $(OBJS) $(PROGRAM)
