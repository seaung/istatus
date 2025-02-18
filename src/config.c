#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/config.h"
#include "dictionary.h"

int load_config(dictionary **ini, const char *filename) {
    *ini = iniparser_load(filename);
    if(*ini == NULL) {
        printf("can't load the config file\n");
        return -1;
    }

    return 0;
}

char* build_status_message(dictionary *ini) {
    char *message = malloc(MAX_MSG_LEN);
    if (!message) return NULL;

    disk_usage disk_stats;
    net_io_counters net_stats;
    cpu_times_percent cpu_stats;
    const char *area;
    const char *address;
    const char *interface;
    time_t current_time;
    char timestamp[32];

    // 获取系统状态数据
    if (!get_disk_usage("/", &disk_stats) ||
        !get_net_io_counters(iniparser_getstring(ini, "network:interface", "en0"), &net_stats) ||
        !get_cpu_times_percent(&cpu_stats)) {
        free(message);
        return NULL;
    }

    // 生成时间戳
    time(&current_time);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // 获取区域和IP地址信息
    area = iniparser_getstring(ini, "app:area", "unknown");
    address = iniparser_getstring(ini, "app:address", "unknown");

    snprintf(message, MAX_MSG_LEN,
        "{\"timestamp\":\"%s\","
        "\"area\":\"%s\","
        "\"address\":\"%s\","
        "\"system_status\":{"
        "\"disk\":{"
        "\"total\":%llu,"
        "\"used\":%llu,"
        "\"free\":%llu,"
        "\"percent\":%.2f"
        "},"
        "\"network\":{"
        "\"bytes_sent\":%llu,"
        "\"bytes_recv\":%llu,"
        "\"packets_sent\":%llu,"
        "\"packets_recv\":%llu"
        "},"
        "\"cpu\":{"
        "\"user\":%.2f,"
        "\"system\":%.2f,"
        "\"idle\":%.2f"
        "}}}",
        timestamp,
        area,
        address,
        disk_stats.total,
        disk_stats.used,
        disk_stats.free,
        disk_stats.percent,
        net_stats.bytes_sent,
        net_stats.bytes_recv,
        net_stats.packets_sent,
        net_stats.packets_recv,
        cpu_stats.user,
        cpu_stats.system,
        cpu_stats.idle);

    return message;
}
