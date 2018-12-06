CC = gcc
CFLAGS = -Wall -pedantic-errors
SRC = main.c
OBJ = main.o

all: $(OBJ)
	$(CC) $(CFLAGS) $(SRC) -o main

clean:
	$(RM) $(OBJ) main