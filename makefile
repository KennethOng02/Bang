dependency=game.o avatar.o deck.o card.o mylib.o player.o interface.o computer_player.o event.o
bin_dir=./bin/
debug_suffix=.debug

debug_dependency=$(addsuffix ${debug_suffix}, ${dependency})
debug_dependency_path=$(addprefix ${bin_dir}, ${debug_dependency})
dependency_path=$(addprefix ${bin_dir}, ${dependency})

all: $(dependency)
	make cardid
	make characterid
	# gcc -fsanitize=address src/main.c $(dependency_path) -lncurses -o bin/main
	gcc src/main.c $(dependency_path) -lncurses -o bin/main

run:
	./bin/main

cardid: mylib.o 
	gcc src/cardid_generator.c bin/mylib.o -lncurses -o bin/cardid_generator
	./bin/cardid_generator

characterid: mylib.o
	gcc src/characterid_generator.c bin/mylib.o -lncurses -o bin/characterid_generator
	./bin/characterid_generator

debug: $(debug_dependency)
	make cardid
	make characterid
	gcc -D__DEBUG__ src/main.c $(debug_dependency_path) -lncurses -o bin/main
	./bin/main 2> log.txt

%.o: src/%.c
	gcc -c $< -o ./bin/$@

%.o.debug: src/%.c
	gcc -c $< -D__DEBUG__ -o ./bin/$@

clean:
	ls -d ./bin/* | grep -v "main" | xargs rm
