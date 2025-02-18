#ifndef __PSUTILS_H_
#define __PSUTILS_H_
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * @brief 磁盘使用情况结构体
 */
typedef struct disk_usage {
    uint64_t total;      /**< 磁盘总容量(字节) */
    uint64_t used;       /**< 已使用容量(字节) */
    uint64_t free;       /**< 可用容量(字节) */
    float percent;       /**< 使用率(百分比) */
} disk_usage;

/**
 * @brief 网络接口流量统计结构体
 */
typedef struct net_io_counters {
    uint64_t bytes_sent;     /**< 发送的字节数 */
    uint64_t bytes_recv;     /**< 接收的字节数 */
    uint64_t packets_sent;   /**< 发送的数据包数 */
    uint64_t packets_recv;   /**< 接收的数据包数 */
    uint64_t errin;         /**< 接收时的错误数 */
    uint64_t errout;        /**< 发送时的错误数 */
    uint64_t dropin;        /**< 接收时丢弃的数据包数 */
    uint64_t dropout;       /**< 发送时丢弃的数据包数 */
} net_io_counters;

/**
 * @brief CPU使用率结构体
 */
typedef struct cpu_times_percent {
    float user;      /**< 用户态CPU使用率 */
    float system;    /**< 系统态CPU使用率 */
    float idle;      /**< CPU空闲率 */
    float nice;      /**< nice优先级的进程CPU使用率 */
    float iowait;    /**< 等待I/O的CPU使用率 */
    float irq;       /**< 处理硬件中断的CPU使用率 */
    float softirq;   /**< 处理软件中断的CPU使用率 */
    float steal;     /**< 被其他虚拟机占用的CPU使用率 */
} cpu_times_percent;

/**
 * @brief 获取指定路径的磁盘使用情况
 * @param path 目标路径
 * @param ret 返回的磁盘使用情况数据
 * @return 成功返回true，失败返回false
 */
bool get_disk_usage(const char path[], disk_usage *ret);

/**
 * @brief 获取指定网络接口的流量统计信息
 * @param interface 网络接口名称
 * @param ret 返回的网络流量统计数据
 * @return 成功返回true，失败返回false
 */
bool get_net_io_counters(const char interface[], net_io_counters *ret);

/**
 * @brief 获取CPU使用率信息
 * @param ret 返回的CPU使用率数据
 * @return 成功返回true，失败返回false
 */
bool get_cpu_times_percent(cpu_times_percent *ret);

#endif
