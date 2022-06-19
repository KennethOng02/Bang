#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "game.h"
#include "player.h"
#include "print.h"

int main(int argc, char ** argv) {
	int player_size = 4;
	int32_t c = 0;
	while((c = getopt( argc, argv, "p:" ) ) != -1 ) {
		switch(c) {
			case 'p':
				player_size = strtod(optarg, NULL);
				if(player_size < 4 || player_size > 7) {
					printf("Only 4 to 7 player number allowed!\n");
					exit(0);
				}
				break;
			case '?':
			default:
				printf( "option: unknown\n" ); 
				break;
		}
	}

	Game_init(player_size);

	Game_run();

	Game_exit();

	return 0;
}
