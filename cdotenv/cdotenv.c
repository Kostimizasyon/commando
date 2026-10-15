#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cdotenv.h"

#define KEY_BUF_SIZE 256
#define VALUE_BUF_SIZE 512

int
read_env(const char *path) {

	FILE* env_file = fopen(path, "r");
	
	if (env_file == NULL) {
		perror(path);
		return FILE_NOT_FOUND;
	}

	char c;

	char key[KEY_BUF_SIZE], val[VALUE_BUF_SIZE];

	int ki = 0, vi = 0;

	// 0 is key 1 is val
	bool active = 0;
	while ( ( c = getc(env_file) ) != EOF) {
		if ( c == ' ') {
			continue;
		}

		else if ( c == '\n') {
			if (ki > 0 && vi > 0) {
				key[ki] = '\0';
				val[vi] = '\0';

				c = setenv(key, val, 1);

				if (c == -1) {
					puts("Unable to set env variables");
					return SET_ENV_FAIL;
				}

			}
			ki = 0;
			vi = 0;

			
			active = 0;

			continue;
		}
		else if ( c == '=' && !active ) {
			active = 1;
		}
		else if (!active) {
			if (ki < KEY_BUF_SIZE )	key[ki++] = c;
			else {
				puts("Possible key overflow! Exiting");
				return KEY_OVERFLOW;
			}
		    }
		else {
			if (vi < VALUE_BUF_SIZE - 1) val[vi++] = c;
			else {
				puts("Possible value overflow! Exiting");
				return VALUE_OVERFLOW;
			}
		}
	}

	fclose(env_file);

	return SUCESS;

}
