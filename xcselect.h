#ifndef __XCSELECT_H__
#define __XCSELECT_H__
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/param.h>
//#include <CoreFoundation/CFBase.h>

#define XC_EXPORT __attribute__((visibility("default")))
#define XC_HIDDEN __attribute__((visibility("hidden")))
#define BUNDLE_PREFIX "com.apple."

typedef struct xcselect_manpaths {
	char **paths;
	uint32_t count;
} xcselect_manpaths;

XC_EXPORT bool xcselect_bundle_is_developer_tool(char *path);
XC_EXPORT bool xcselect_developer_dir_matches_path(const char *developer_dir, const char *path);
XC_EXPORT bool xcselect_get_developer_dir_path(char *buffer, int buffer_size,
				bool *was_environment, bool *was_cltools,
				bool *was_default);
XC_EXPORT bool xcselect_find_developer_contents_from_path(char *path, char *buffer,
				size_t buffer_size, bool *was_cltools);
XC_EXPORT xcselect_manpaths *xcselect_get_manpaths(char *sysroot);
XC_EXPORT char *xcselect_get_version(void);
//XC_EXPORT errno_t xcselect_host_sdk_path(/*xcselect_host_sdk_policy_t*/int sdk_policy,
//				char * __nullable * __nonnull path);
XC_EXPORT void xcselect_manpaths_free(xcselect_manpaths *xcp);
XC_EXPORT uint32_t xcselect_manpaths_get_num_paths(xcselect_manpaths *xcp);
XC_EXPORT const char *xcselect_manpaths_get_path(xcselect_manpaths *xcp, unsigned i);
XC_EXPORT xcselect_manpaths *xcselect_get_manpaths(char *sdkname);

#ifdef _XC_INTN
XC_HIDDEN void path_append(char *path_dest, size_t length, const char *path_to_join);
XC_HIDDEN void path_join(char *path_dest, size_t length, const char *path_src, const char *path_extra);
XC_HIDDEN bool path_exists(char *path);
XC_HIDDEN bool path_contains_xcrun(const char *path);
XC_HIDDEN bool path_is_dir(const char *path);
XC_HIDDEN bool str_endswith(const char *str, const char *end);
XC_HIDDEN bool get_developer_dir_from_symlink(const char *path, char *buffer, int buffer_size, bool *status);
XC_HIDDEN void xcselect_manpaths_append(xcselect_manpaths* paths, const char* path);
XC_HIDDEN bool is_path_xcrun_shim(const char *path);
#endif

#endif
