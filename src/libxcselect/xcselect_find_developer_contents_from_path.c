#include <xcselect.h>

XC_EXPORT
bool xcselect_find_developer_contents_from_path(char *path, char *buffer, size_t buffer_size, bool *was_cltools) {
	char str[1024];
	if (*path == '/') {
		// the first char of path is '/'
		sprintf(str, "%s", path);
	} else {
		getcwd(str, 1024);
		path_append(str, 1024, path);
	}
	for (char *i = str + strlen(str) - 1; i > str && *i == '/'; --i) {
		*i = 0; // '\0'
	}
	char joinedPath[1024];
	if(str[0] == '/' || (path_join(joinedPath, 1024, str, "mach_kernel"),/*BOOL*/path_exists(joinedPath))) {
		path_join(buffer , buffer_size, str, "Library/Developer/CommandLineTools");
		if(path_contains_xcrun(buffer)) {
			*was_cltools=1;
			return true;
		}
		return false;
	}
	char *v10 = strrchr(str,'/');
	if(v10 && strcmp(v10 + 1, "CommandLineTools") == 0) { // e.g. /a/b/c/CommandLineTools
		strlcpy(buffer, str, buffer_size);
		if(path_contains_xcrun(buffer)) {
			*was_cltools = 1;
			return true;
		}
		return false;
	}
	path_join(buffer, buffer_size, str, "Contents/Developer");
	if(!path_is_dir(buffer)) {
		if(path_contains_xcrun(str)) {
			strlcpy(buffer, str, buffer_size);
		} else {
			path_join(buffer, buffer_size, str, "Xcode.app/Contents/Developer");
			if(!path_is_dir(buffer)) {
				return false;
			}
		}
	}
	*was_cltools = 0;
	return true;
}
