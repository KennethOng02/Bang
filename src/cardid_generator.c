#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "print.h"
#include "mylib.h"

int main () {
	// generating 'cardid.h' according 'card.txt'
	FILE *pfile = fopen("src/card.txt", "r");
	assert(pfile);

	char *idFilename = "src/cardid.h";
	FILE *pIDFile = fopen(idFilename, "w");
	assert(pIDFile);
	
	int lineNum = 0;

	int bufSize = 1024;
	char *buffer = calloc(bufSize, sizeof(char));
	
	snprintf(buffer, bufSize, "#pragma once\n#include \"card.h\"\n");
	if ( fwrite( buffer, 1, strlen(buffer), pIDFile ) != strlen(buffer) ) {
		ERROR_PRINT("Failed to write file: %s\n", idFilename);
	}
	while(fgets(buffer, bufSize, pfile) != NULL) {
		
		if ( buffer[strlen(buffer)-1] == '\n' ) {
			buffer[strlen(buffer)-1] = '\0';
		}

		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, ",");

		if ( counter == 1 ) {

			snprintf(buffer, bufSize, "#define %s %d\n", line[0], lineNum);

		} else if ( counter == 6 ) {

			char *card_str = line[4];
			char **card_list;
			int cardCounter;
			mystrsplit(&card_list, &cardCounter, card_str, NULL);

			if ( cardCounter != strtod(line[3], NULL) ) {
				ERROR_PRINT("number of card inconsistent with suit list in line %d\n", lineNum);
			}

			snprintf(buffer, bufSize, "#define %s %d\n", line[1], lineNum);

			for ( int i=0; i<cardCounter; i++ ) {
				free(card_list[i]);
			}
			free(card_list);

		} else {
			ERROR_PRINT("Invalid format in line %d\n", lineNum);
		}

		if ( fwrite( buffer, 1, strlen(buffer), pIDFile ) != strlen(buffer) ) {
			ERROR_PRINT("Failed to write file: %s\n", idFilename);
		}

		lineNum++;

		for ( int i=0; i<counter; i++ ) {
			free(line[i]);
		}
		free(line);
	}

	fseek(pfile, 0, SEEK_SET);
	
	snprintf(buffer, bufSize, "static int (*Card_funArr[])(Avatar *,Avatar *, Game *, Card *) = { ");
	if ( fwrite( buffer, 1, strlen(buffer), pIDFile ) != strlen(buffer) ) {
		ERROR_PRINT("Failed to write file: %s\n", idFilename);
	}

	while(fgets(buffer, bufSize, pfile) != NULL) {
		if ( buffer[strlen(buffer)-1] == '\n' ) {
			buffer[strlen(buffer)-1] = '\0';
		}
		char **line;
		int counter;
		
		mystrsplit(&line, &counter, buffer, ",");
		
		if ( counter == 6 ) {
			snprintf(buffer, bufSize, "&play_%s, ", line[1]);
		} else {
			snprintf(buffer, bufSize, "NULL, ");
		}

		if ( fwrite( buffer, 1, strlen(buffer), pIDFile ) != strlen(buffer) ) {
			ERROR_PRINT("Failed to write file: %s\n", idFilename);
		}

		for ( int i=0; i<counter; i++ ) {
			free(line[i]);
		}
		free(line);
	}
	snprintf(buffer, bufSize, "};");
	if ( fwrite( buffer, 1, strlen(buffer), pIDFile ) != strlen(buffer) ) {
		ERROR_PRINT("Failed to write file: %s\n", idFilename);
	}

	fclose(pfile);
	fclose(pIDFile);
	free(buffer);
	return 0;
}
