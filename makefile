dependency=player.o card.o character.o equipment.o game.o mylib.o
bin_dir=./bin/

dependency_path=$(addprefix ${bin_dir}, ${dependency})

all: $(dependency)
	gcc src/main.c $(dependency_path) -o bin/main

run:
	gcc src/main.c -o bin/main
	./bin/main

%.o: src/%.c
	gcc -c $< -o ./bin/$@
