#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "iniparser.h"
#include "psutil.h"

#define MAX_MSG_LEN 4096

int load_config(dictionary **ini, const char *filename);
char* build_status_message(dictionary *ini);

#endif
