#include <xcrun.h>

XC_HIDDEN
void PrintUsageAndExit(int ret)
{
	char *progname = getprogname();
	fprintf(stderr,"Usage: %s [options] <tool name> ... arguments ...\n", progname);
	fputc('\n', stderr);
	fwrite("Find and execute the named command line tool from the active developer\ndirectory.\n\nThe active developer directory can be set using `xcode-select`, or via the\nDEVELOPER_DIR environment variable. See the xcrun and xcode-select manual\npages for more information.\n", 260, 1, stderr);
	fputc('\n', stderr);
	fwrite("Options:\n", 9, 1, stderr);
	fprintf(stderr, "  %-*s %s\n", 27, "-h, --help", "show this help message and exit");
	fprintf(stderr, "  %-*s %s\n", 27, "--version", "show the xcrun version");
	fprintf(stderr, "  %-*s %s\n", 27, "-v, --verbose", "show verbose logging output");
	fprintf(stderr, "  %-*s %s\n", 27, "--sdk <sdk name>", "find athe tool for the given SDK name");
	fprintf(stderr, "  %-*s %s\n", 27, "--toolchain <name>", "find the tool for the given toolchain");
	fprintf(stderr, "  %-*s %s\n", 27, "-l, --log", "show commands to be executed (with --run)");
	fprintf(stderr, "  %-*s %s\n", 27, "-f, --find", "only find and print the tool path");
	fprintf(stderr, "  %-*s %s\n", 27, "-r, --run", "find and execute the tool (the default behavior)");
	fprintf(stderr, "  %-*s %s\n", 27, "-n, --no-cache", "do not use the lookup cache");
	fprintf(stderr, "  %-*s %s\n", 27, "-k, --kill-cache", "invalidate all existing cache entries");
	fprintf(stderr, "  %-*s %s\n", 27, "--show-sdk-path", "show selected SDK install path");
	fprintf(stderr, "  %-*s %s\n", 27, "--show-sdk-version", "show selected SDK version");
	fprintf(stderr, "  %-*s %s\n", 27, "--show-sdk-build-version", "show selected SDK build version");
	fprintf(stderr, "  %-*s %s\n", 27, "--show-sdk-platform-path", "show selected SDK platform path");
	fprintf(stderr, "  %-*s %s\n", 27, "--show-sdk-platform-version", "show selected SDK platform version");
	exit(ret);
}

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
	if ((*path != '\0') && (stat(path, &st) == 0) && ((st.st_mode & 0xf040) == 0x8040)) return true;
	return false;
}

XC_HIDDEN
bool strendswith(const char *str, const char *end)
{
        size_t str_size = strlen(str);
        size_t end_size = strlen(end);
        if ((str_size < end_size) || (memcmp(str + (str_size - end_size), end, end_size + 1) != 0)) {
                return false;
        }
        return true;
}

XC_HIDDEN
char *msprintf(char *str, ...)
{
	char *buffer;
	va_list ap;
	va_start(ap, str);
	vasprintf(&buffer, str, ap);
	va_end(ap);
	return buffer;
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
			return (bool)(0x3fffffffbffffffeU >> (*env_value - '0' & '?') & 0xffffffffffffff01);
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
		if (*path_dev == '/') {
			path_combine = msprintf("%.*s/%s", path_multi, path_dev, tool_name);
			if (path_is_executable_file(path_combine) && is_path_xcrun_shim(path_combine) == 0) {
				return path_combine;
			}
			free(path_combine);
		}
		path_dev = path_multi + 1;
		if (!path_multi) {
			return NULL;
		}
	} while (true);
}
