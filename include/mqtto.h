#ifndef __MQTTO_H_
#define __MQTTO_H_
#include "MQTTClient.h"
#include "MQTTAsync.h"

void connect_lost(void *context, char *cause);

void delivered(void *context, MQTTClient_deliveryToken token);

int connect_mqtt(MQTTClient *client, const char *uri, const char *client_id, unsigned int keepalive);

int message_arrived(void *context, char *topic, int topic_len, MQTTClient_message *message);

int publish_message(MQTTClient client, const char *topic, int qos, char *msg, int len);

#endif
