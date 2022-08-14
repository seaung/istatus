#ifndef __PSUTILS_H_
#define __PSUTILS_H_
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct disk_usage {
	uint64_t total;
	uint64_t used;
	uint64_t free;
	float percent;
} disk_usage;


bool get_disk_usage(const char[], disk_usage *);

#endif
