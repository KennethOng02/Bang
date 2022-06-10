all: mysplit.o
	gcc main.c mysplit.o -o main

%.o : %.c
	gcc -c $< -o $@
