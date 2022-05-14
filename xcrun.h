#ifndef __LIBXCRUN__
#define __LIBXCRUN__
#include <unistd.h>
#include <Block.h>
#include <stdarg.h>

#define XC_EXPORT __attribute__((visibility("default")))
#define XC_HIDDEN __attribute__((visibility("hidden")))

#define XCRUN_VER "59"

XC_EXPORT char *xcrun_get_version(void);
XC_EXPORT void *xcrun_iter_manpaths(const char* devpath, const char* sdkname, const char *path);
XC_EXPORT int xcrun_main(int argc, char *argv[]);
XC_EXPORT void *xcrun_set_unknown_utility_handler(const void *block_invoke);

XC_HIDDEN int xcrun_main2(char *tool_name, int argc, char *argv[], char *devdir);
XC_HIDDEN bool util_get_bool_from_environment(const char *env);
XC_HIDDEN void path_clean_copy(char *path);
XC_HIDDEN char *msprintf(char *str, ...);

#endif
