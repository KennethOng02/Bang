#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "debug.h"
#include "mylib.h"

int main () {
	// generating 'characterid.h' according 'character.txt'
	FILE *pfile = fopen("src/character.txt", "r");
	assert(pfile);

	char *idFilename = "src/characterid.h";
	FILE *pIDFile = fopen(idFilename, "w");
	assert(pIDFile);
	
	int lineNum = 0;

	int bufSize = 1024;
	char *buffer = calloc(bufSize, sizeof(char));
	
	snprintf(buffer, bufSize, "#pragma once\n\n");
	if ( fwrite( buffer, 1, strlen(buffer), pIDFile ) != strlen(buffer) ) {
		ERROR_PRINT("Failed to write file: %s\n", idFilename);
	}
	while(fgets(buffer, bufSize, pfile) != NULL) {
		
		if ( buffer[strlen(buffer)-1] == '\n' ) {
			buffer[strlen(buffer)-1] = '\0';
		}

		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, "\"");

		if (counter != 7 ) {
			ERROR_PRINT("Invalid format in line %d\n", lineNum);
		}

        char *name = malloc(strlen(line[1])+1);
        strcpy(name, line[1]);
        for ( int i=0; i<strlen(name); i++ ) {
            if (name[i] == ' ') name[i] = '_';
        }
        
		snprintf(buffer, bufSize, "#define %s %d\n", name, lineNum);
		if ( fwrite( buffer, 1, strlen(buffer), pIDFile ) != strlen(buffer) ) {
			ERROR_PRINT("Failed to write file: %s\n", idFilename);
		}

		lineNum++;

		for ( int i=0; i<counter; i++ ) {
			free(line[i]);
		}
		free(line);
	}
}