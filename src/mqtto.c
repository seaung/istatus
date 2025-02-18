#include <stdio.h>
#include <time.h>
#include "../include/mqtto.h"

// 全局变量，用于跟踪消息投递状态
volatile MQTTClient_deliveryToken delivery_token;

/**
 * MQTT连接丢失回调函数
 * @param context 上下文指针，可用于传递用户数据
 * @param cause 连接丢失原因
 */
void connect_lost(void *context, char *cause) {
	printf("connect lost, the reason : %s\n", cause);
}

/**
 * 消息投递完成回调函数
 * @param context 上下文指针，可用于传递用户数据
 * @param token 消息投递令牌
 */
void delivered(void *context, MQTTClient_deliveryToken token) {
	printf("message with token value %d delivery confirmed\n", token);
	delivery_token = token;
}

/**
 * 发布MQTT消息
 * @param client MQTT客户端实例
 * @param topic 目标主题
 * @param qos 服务质量等级(0,1,2)
 * @param msg 消息内容
 * @param len 消息长度
 * @return 0表示成功，-1表示失败，-2表示超时
 */
int publish_message(MQTTClient client, const char *topic, int qos, char *msg, int len) {
	MQTTClient_deliveryToken token;
	MQTTClient_message publish_msg = MQTTClient_message_initializer;
	time_t start_time;
	const int timeout = 10; // 10秒超时

	if(client == NULL)
		return -1;

	publish_msg.payload = msg;
	publish_msg.payloadlen = len;
	publish_msg.qos = qos;
	publish_msg.retained = 0;
	delivery_token = 0;

	MQTTClient_publishMessage(client, topic, &publish_msg, &token);
	start_time = time(NULL);

	// 等待消息投递完成或超时
	while(delivery_token != token) {
		if(time(NULL) - start_time >= timeout) {
			return -2; // 超时返回
		}
	}

	return 0;
}

/**
 * 消息到达回调函数
 * @param context 上下文指针，可用于传递用户数据
 * @param topic 消息主题
 * @param topic_len 主题长度
 * @param message MQTT消息结构体
 * @return 1表示消息处理成功
 */
int message_arrived(void *context, char *topic, int topic_len, MQTTClient_message *message) {
	int index;
	char *payloads;

	printf("Received message on topic: %s\n", topic);
	payloads = message->payload;
	printf("Message content: ");
	for(index = 0; index < message->payloadlen; index++) {
		putchar(*payloads++);
	}
	putchar('\n');

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topic);
	return 1;
}

/**
 * 连接MQTT服务器
 * @param client MQTT客户端实例指针
 * @param uri 服务器URI，格式为"tcp://host:port"
 * @param client_id 客户端标识符
 * @param keepalive 保活时间间隔(秒)
 * @return 0表示连接成功，-1表示连接失败
 */
int connect_mqtt(MQTTClient *client, const char *uri, const char *client_id, unsigned int keepalive) {
	int code;

	MQTTClient_connectOptions connect_options = MQTTClient_connectOptions_initializer;
	MQTTClient_create(client, uri, client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTClient_setCallbacks(client, NULL, connect_lost, message_arrived, delivered);

	// 配置连接选项
	connect_options.keepAliveInterval = keepalive;
	connect_options.cleansession = 0; // 使用持久会话
	connect_options.reliable = 1; // 使用可靠传输

	if((code = MQTTClient_connect(client, &connect_options)) != MQTTCLIENT_SUCCESS) {
		printf("Failed to connect, return code %d\n", code);
		return -1;
	}

	printf("Successfully connected to MQTT broker\n");
	return 0;
}
