dependency=game.o avatar.o deck.o card.o mylib.o
bin_dir=./bin/

dependency_path=$(addprefix ${bin_dir}, ${dependency})

all: $(dependency)
	gcc src/main.c $(dependency_path) -o bin/main

run:
	./bin/main

debug: $(dependency)
	gcc src/main.c $(dependency_path) -D__DEBUG__ -o bin/main

%.o: src/%.c
	gcc -c $< -o ./bin/$@

clean:
	rm ./bin/?*
