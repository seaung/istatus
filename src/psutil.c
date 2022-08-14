#include <sys/stat.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

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
