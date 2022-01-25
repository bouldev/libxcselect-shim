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

XC_HIDDEN
bool path_is_executable_file(char *path)
{
	struct stat st;
	if ((*path !== '\0') && (stat(path, &st) == 0) && ((st.st_mode & 0xf040) == 0x8040)) return true;
	return false;
}

XC_HIDDEN
char *msprintf(char *str, ...)
{
	char *buffer;
	va_list ap;
	va_start(ap, str);
	vasprintf(&buffer, str, ap);
	va_end(ap);
	return buffer
}

XC_HIDDEN
int syserror(const char *str, ...)
{
	va_list ap;
	va_start(ap, str);
	fprintf(stderr, "%s: error: ", getprogname());
	vfprintf(stderr, str, ap);
	fprintf(stderr, " (errno=%s)\n", strerror(errno));
	va_end(ap);
	return errno;
}

XC_HIDDEN
void diagnostic(const char *warn_prefix, const char *warn_content, ...)
{
	va_list ap;
	va_start(ap, warn_content);
	fprintf(stderr, "%s: %s: ", getprogname(), warn_prefix);
	vfprintf(stderr, warn_content, ap);
	fputc('\n', stderr);
	va_end(ap);
	// Maybe errno_t return
}

XC_HIDDEN
void warning(const char *str, ...)
{
	va_list ap;
	va_start(ap, str);
	diagnostic("warning", str, ap);
	va_end(ap);
}

XC_HIDDEN
bool util_get_bool_from_environment(const char *env)
{
	char *env_value = getenv(env);
	if (env_value) {
		if (*env_value - '0' < '?') {
			return (bool)(0x3fffffffbffffffeU >> (*env_value - '0' & '?') & 0xffffffffffffff01)
		}
		return true;
	}
	return false;
}

XC_HIDDEN
bool is_path_xcrun_shim(char *path)
{
	return 0; // return 0 on success
}

XC_HIDDEN
char *util_find_executable_in_paths_string(const char *tool_name, char *path_dev)
{
	size_t path_len;
	char *path_multi = strrchr(path_dev, ':');
	char *path_combine;
	do {
		if (!path_multi) {
			path_len = strlen(path_dev);
		} else {
			path_len = (long)path_multi - (long)path_dev;
		}
		if (*path_dev = '/') {
			path_combine = msprintf("%.*s/%s", path_multi, path_dev, tool_name);
			if (path_is_executable_file(path_combine) && is_path_xcrun_shim(path_combine) == 0) {
				return path_combine;
			}
			free(path_combine);
		}
		path_dev = path_multi + 1;
		if (!path_multi) {
			return nil;
		}
	} while (true);
}
