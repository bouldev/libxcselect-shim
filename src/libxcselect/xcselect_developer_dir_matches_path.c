#include <xcselect.h>
#include <string.h>

XC_EXPORT
bool xcselect_developer_dir_matches_path(const char *developer_dir,const char *path) {
	char str[1024];
	if (developer_dir) {
		strlcpy(str, developer_dir, MAXPATHLEN);
	} else {
		bool ignoringPool;
		if (!xcselect_get_developer_dir_path(str, 1024, &ignoringPool, &ignoringPool, &ignoringPool)) {
			return false;
		}
	}
	size_t len = strlen(str);
	if (!len) return false;
	for (char *i = &str[len - 1]; i > str; *i-- = 0 ) {
		if (*i != '/') break;
	}
	len = strlen(str);
	if (str_endswith(str, ".app/Contents/Developer")) {
		str[len - 19] = 0;
	}
	len = strlen(str);
	if (path && strlen(path) >= len) {
		return strncmp(str, path, len) == 0;
	}
	return false;
}
