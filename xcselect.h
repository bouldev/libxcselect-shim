#ifndef _LIBXCSELECT_H_
#define _LIBXCSELECT_H_
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define XC_EXPORT __attribute__((visibility("default")))
#define XC_HIDDEN __attribute__((visibility("hidden")))
#define BUNDLE_PREFIX "com.apple."

XC_EXPORT bool xcselect_bundle_is_developer_tool(char *path);
XC_EXPORT bool xcselect_developer_dir_matches_path(const char *path_orig, char *path_cmp);
XC_EXPORT bool xcselect_get_developer_dir_path(char *buffer, int buffer_size,
				bool *was_environment, bool *was_cltools,
				bool *was_default);
XC_EXPORT bool xcselect_find_developer_contents_from_path(char *path, char *buffer,
				size_t buffer_size, bool *was_cltools);

#ifdef _XC_INTN
void path_append(char *path_dest, size_t length, const char *path_to_join);
void path_join(char *path_dest, size_t length, const char *path_src, const char *path_extra);
bool path_exists(char *path);
bool path_contains_xcrun(const char *path);
bool path_is_dir(const char *path);
bool is_path_xcrun_shim(const char *path);
#endif

#endif
