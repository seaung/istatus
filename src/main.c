#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include "dictionary.h"
#include "iniparser.h"
#include "../include/mqtto.h"
#include "../include/config.h"

#define VERSION "1.0.0#dev"
#define URI_MAX_LEN 128
#define REPORT_INTERVAL 60

// 全局变量
volatile int g_running = 1;
MQTTClient g_client;

// 信号处理函数
void signal_handler(int signo) {
    if (signo == SIGINT || signo == SIGTERM) {
        g_running = 0;
    }
}

int main(int argc, char *argv[]) {
    int iret;
    int index_options = 0;
    int coption = 0;
    int port;
    int keepalive;

    char *options_string = "c:vh";
    char *conf = NULL;
    const char *host;
    const char *topic;
    char uri[URI_MAX_LEN];
    char *message;

    dictionary *ini = NULL;

    static struct option long_options[] = {
        {"config", required_argument, NULL, '-'},
        {"version", no_argument, NULL, '-'},
        {"help", no_argument, NULL, '-'}
    };

    // 设置信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // 解析命令行参数
    while ((coption = getopt_long(argc, argv, options_string, long_options, &index_options)) != -1) {
        switch(coption) {
            case '-':
                switch(index_options) {
                    case 0:
                        if(optarg) {
                            conf = strdup(optarg);
                            fprintf(stderr, "--config %s\n", conf);
                        } else {
                            fprintf(stderr, "--config no param\n");
                            return -1;
                        }
                        break;
                    case 1:
                        fprintf(stderr, "version : %s\n", VERSION);
                        return 0;
                    case 2:
                        fprintf(stderr, "Usage : %s[c:vh]\n"
                                "--config , -c \n config file name\n"
                                "--version , -v \n version number\n"
                                "--help , -h \n print help information\n", argv[0]);
                        return 0;
                    default:
                        fprintf(stderr, "options error !\n");
                        break;
                }
                break;
            case 'c':
                if(optarg) {
                    conf = strdup(optarg);
                    fprintf(stderr, "-c %s\n", conf);
                } else {
                    fprintf(stderr, "-c no param\n");
                    return -1;
                }
                break;
            case 'v':
                fprintf(stderr, "version : %s\n", VERSION);
                return 0;
            case 'h':
                fprintf(stderr, "Usage : %s[c:vh]\n"
                        "--config , -c \n config file name\n"
                        "--version , -v \n version number\n"
                        "--help , -h \n print help information\n", argv[0]);
                return 0;
            default:
                fprintf(stderr, "please input options\n");
                break;
        }
    }

    if(optind < argc) {
        fprintf(stderr, "non-option ARGV-elements:");
        while(optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }

    if(NULL == conf) {
        fprintf(stderr, "please input config file path\n");
        return -1;
    }

    printf("Loading config file: %s\n", conf);
    
    // 加载配置文件
    ini = iniparser_load(conf);
    if (ini == NULL) {
        fprintf(stderr, "Failed to load config file\n");
        free(conf);
        return -1;
    }
    
    // 获取MQTT配置
    host = iniparser_getstring(ini, "mqtt:host", "127.0.0.1");
    topic = iniparser_getstring(ini, "mqtt:topic", "status");
    port = iniparser_getint(ini, "mqtt:port", 1883);
    keepalive = iniparser_getint(ini, "mqtt:keepalive", 600);
    
    // 构建MQTT服务器URI
    snprintf(uri, URI_MAX_LEN, "tcp://%s:%d", host, port);
    
    // 连接MQTT服务器
    iret = connect_mqtt(&g_client, uri, "istatus", keepalive);
    if (iret == -1) {
        fprintf(stderr, "Failed to connect to MQTT server\n");
        iniparser_freedict(ini);
        free(conf);
        return -1;
    }
    
    printf("Successfully connected to MQTT server\n");
    
    // 主循环：定期采集和上报系统状态
    while (g_running) {
        // 构建状态消息
        message = build_status_message(ini);
        if (message) {
            // 发布状态消息
            publish_message(g_client, topic, 0, message, strlen(message));
            free(message);
        }
        
        sleep(REPORT_INTERVAL);
    }
    
    // 清理资源
    MQTTClient_disconnect(g_client, 10000);
    MQTTClient_destroy(&g_client);
    iniparser_freedict(ini);
    free(conf);
    
    printf("Program terminated gracefully\n");
    return 0;
}
