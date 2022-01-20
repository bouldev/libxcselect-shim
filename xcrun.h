#ifndef __LIBXCRUN__
#define __LIBXCRUN__
#include <unistd.h>

#define XC_EXPORT __attribute__((visibility("default")))
#define XC_HIDDEN __attribute__((visibility("hidden")))

#define XCRUN_VER "59"

XC_EXPORT char *xcrun_get_version(void);

#endif
