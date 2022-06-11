all: 
	gcc src/main.c -o bin/main

run:
	gcc src/main.c -o bin/main
	./bin/main
