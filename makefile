dependency=game.o avatar.o deck.o card.o mylib.o
bin_dir=./bin/
debug_suffix=.debug

debug_dependency=$(addsuffix ${debug_suffix}, ${dependency})
debug_dependency_path=$(addprefix ${bin_dir}, ${debug_dependency})
dependency_path=$(addprefix ${bin_dir}, ${dependency})

all: $(dependency)
	gcc src/main.c $(dependency_path) -o bin/main

run:
	./bin/main

debug: $(debug_dependency)
	gcc -D__DEBUG__ src/main.c $(debug_dependency_path) -o bin/main

%.o: src/%.c
	gcc -c $< -o ./bin/$@

%.o.debug: src/%.c
	gcc -c $< -D__DEBUG__ -o ./bin/$@

clean:
	ls -d ./bin/* | grep -v "main" | xargs rm
