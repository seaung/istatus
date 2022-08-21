#include <stdio.h>
#include "../include/mqtto.h"

volatile MQTTClient_deliveryToken delivery_token;

void connect_lost(void *context, char *cause) {
	printf("connect lost, the reason : %s\n", cause);
}

void delivered(void *context, MQTTClient_deliveryToken token) {
	printf("message with token value %d delivery confirmed\n", token);
	delivery_token = token;
}

int publish_message(MQTTClient client, const char *topic, int qos, char *msg, int len) {
	MQTTClient_deliveryToken token;
	MQTTClient_message publish_msg = MQTTClient_message_initializer;

	if(client == NULL)
		return -1;

	publish_msg.payload = msg;
	publish_msg.payloadlen = len;
	publish_msg.qos = qos;
	publish_msg.retained = 0;
	delivery_token = 0;
	MQTTClient_publishMessage(client, topic, &publish_msg, &token);

	while(delivery_token != token);

	return 0;
}

int message_arrived(void *context, char *topic, int topic_len, MQTTClient_message *message) {
	int index;
	char *payloads;

	payloads = message->payload;
	for(index = 0; index < message->payloadlen; index++) {
		putchar(*payloads++);
	}
	putchar('\n');

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topic);
	return 1;
}

int connect_mqtt(MQTTClient *client, const char *uri, const char *client_id, unsigned int keepalive) {
	int code;

	MQTTClient_connectOptions connect_options = MQTTClient_connectOptions_initializer;
	MQTTClient_create(client, uri, client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTClient_setCallbacks(client, NULL, connect_lost, message_arrived, delivered);
	connect_options.keepAliveInterval = keepalive;
	connect_options.cleansession = 0;

	if((code = MQTTClient_connect(client, &connect_options)) != MQTTCLIENT_SUCCESS) {
		return -1;
	}

	return 0;
}
