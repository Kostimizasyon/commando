#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cdotenv.h"

#define KEY_BUF_SIZE 256
#define VALUE_BUF_SIZE 512

void
read_env(const char *path) {

	FILE* env_file = fopen(path, "r");
	
	if (env_file == NULL) {
		puts("File not found");
		return;
	}
	char c;

	char key[KEY_BUF_SIZE], val[VALUE_BUF_SIZE];

	int ki, vi;

	// 0 is key 1 is val
	bool active = 0;
	while ( ( c = getc(env_file) ) != EOF) {

		if ( c == ' ') {
			continue;
		}
		else if ( c == '\n') {
			if (ki > 0 && vi > 0) {
				c = setenv(key, val, 1);

				if (c == -1) {
					puts("unable to set env variables");
					return;
				}
			}
			ki = 0;
			vi = 0;

			key[ki] = '\0';
			val[vi] = '\0';
			
			active = 0;

			continue;
		}
		else if ( c == '=' && !active ) {
			active = 1;
		}
		else if (!active) {
			if (ki < KEY_BUF_SIZE - 1) key[ki++] = c;
			else {
				puts("Possible key overflow! Exiting");
				return;
			}
		    }
		else {
			if (vi < VALUE_BUF_SIZE - 1) val[vi++] = c;
			else {
				puts("Possible value overflow! Exiting");
				return;
			}
		}
	}

	fclose(env_file);

}
