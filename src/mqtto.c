#include <stdio.h>
#include "../include/mqtto.h"

void connect_lost(void *context, char *cause) {
	printf("connect lost, the reason : %s\n", cause);
}
