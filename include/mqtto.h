#ifndef __MQTTO_H_
#define __MQTTO_H_
#include "MQTTClient.h"

void connect_lost(void *context, char *cause);

int connect_mqtt(MQTTClient *client, const char *uri, unsigned int client_id);

#endif
