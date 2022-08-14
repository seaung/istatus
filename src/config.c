#include <stdio.h>
#include "../include/config.h"
#include "dictionary.h"

int load_config(dictionary *ini, const char *filename) {
	ini = iniparser_load(filename);
	if(ini == NULL) {
		printf("can't load the config file\n");
		return -1;
	}

	return 0;
}
