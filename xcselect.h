#ifndef _LIBXCSELECT_H_
#define _LIBXCSELECT_H_
#include <stdbool.h>

bool xcselect_bundle_is_developer_tool(char* path);
bool xcselect_developer_dir_matches_path(const char* path_orig, char* path_cmp);
bool xcselect_get_developer_dir_path(char *buffer, int buffer_size,
				bool *was_environment, bool *was_cltools,
				bool *was_default);
#endif
