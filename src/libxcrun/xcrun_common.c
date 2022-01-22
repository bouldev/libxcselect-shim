#include <xcrun.h>

XC_HIDDEN
void path_clean_copy(char *path)
{
	bool path_clean;
	size_t path_len = strlen(path);
	if (1 < (long)path_len) {
		do {
			if ('/' < path[path_len - 1] || (1 << (path[path_len - 1] & '?') & LONG_MIN) == 0) break;
			path_clean = 2 < (long)path_len;
			path_len = path_len - 1;
		} while (path_clean);
		path_len = 1;
	}
	strndup(path, path_len);
}
