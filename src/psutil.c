#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ifaddrs.h>

#include "../include/psutil.h"

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...)                                                        \
  fprintf(stderr, "[ERROR] (%s:%d:%s: errno: %d, %s) " M "\n", __FILE__,       \
          __LINE__, __FUNCTION__, errno, clean_errno(), ##__VA_ARGS__)

#define check(A, M, ...)                                                       \
  if (!(A)) {                                                                  \
    log_err(M, ##__VA_ARGS__);                                                 \
    errno = 0;                                                                 \
    goto error;                                                                \
  }

float percentage(uint64_t n, uint64_t d) {
  float percent = ((float)n / (float)d) * 100.0;
  return percent;
}

bool get_disk_usage(const char path[], disk_usage *ret) {
    int res;
    struct statvfs s;

    res = statvfs(path, &s);
    check(res == 0, "error in calling statvfs for %s", path);
    ret->free = s.f_bavail * s.f_frsize;
    ret->total = s.f_blocks * s.f_frsize;
    ret->used = (s.f_blocks - s.f_bfree) * s.f_frsize;
    ret->percent = percentage(ret->used, ret->total);

    return true;
error:
    return false;
}

bool get_net_io_counters(const char interface[], net_io_counters *ret) {
    struct ifaddrs *ifaddr, *ifa;
    struct if_data *if_data;

    if (getifaddrs(&ifaddr) == -1) {
        log_err("Failed to get interface addresses");
        return false;
    }

    bool found = false;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_data == NULL)
            continue;

        if (strcmp(ifa->ifa_name, interface) == 0 && 
            ifa->ifa_addr->sa_family == AF_LINK) {
            if_data = (struct if_data *)ifa->ifa_data;
            
            ret->bytes_sent = if_data->ifi_obytes;
            ret->bytes_recv = if_data->ifi_ibytes;
            ret->packets_sent = if_data->ifi_opackets;
            ret->packets_recv = if_data->ifi_ipackets;
            ret->errin = if_data->ifi_ierrors;
            ret->errout = if_data->ifi_oerrors;
            ret->dropin = if_data->ifi_iqdrops;
            ret->dropout = 0; // MacOS doesn't track output drops
            
            found = true;
            break;
        }
    }

    freeifaddrs(ifaddr);
    if (!found) {
        log_err("Interface %s not found", interface);
        return false;
    }
    return true;
}

bool get_cpu_times_percent(cpu_times_percent *ret) {
    static uint64_t last_user = 0, last_nice = 0, last_system = 0;
    static uint64_t last_idle = 0, last_total = 0;
    
    int mib[] = { CTL_KERN, KERN_CPTIME };
    uint64_t cpu_times[CPUSTATES];
    size_t len = sizeof(cpu_times);
    
    if (sysctl(mib, 2, &cpu_times, &len, NULL, 0) == -1) {
        log_err("Failed to get CPU times");
        return false;
    }
    
    uint64_t user = cpu_times[CP_USER];
    uint64_t nice = cpu_times[CP_NICE];
    uint64_t system = cpu_times[CP_SYS];
    uint64_t idle = cpu_times[CP_IDLE];
    uint64_t total = user + nice + system + idle;
    
    if (last_total > 0) {
        uint64_t total_diff = total - last_total;
        if (total_diff > 0) {
            ret->user = ((float)(user - last_user) / total_diff) * 100.0;
            ret->nice = ((float)(nice - last_nice) / total_diff) * 100.0;
            ret->system = ((float)(system - last_system) / total_diff) * 100.0;
            ret->idle = ((float)(idle - last_idle) / total_diff) * 100.0;
        }
    }
    
    // MacOS doesn't provide these metrics
    ret->iowait = 0.0;
    ret->irq = 0.0;
    ret->softirq = 0.0;
    ret->steal = 0.0;
    
    last_user = user;
    last_nice = nice;
    last_system = system;
    last_idle = idle;
    last_total = total;
    
    return true;
}
