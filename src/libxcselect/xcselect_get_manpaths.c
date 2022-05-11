#include <xcselect.h>

extern int _xpc_runtime_is_app_sandboxed(void);

XC_EXPORT
xcselect_manpaths *xcselect_get_manpaths(char *sdkname)
{
	char path[MAXPATHLEN], path_xcrun[MAXPATHLEN], path_to_append[MAXPATHLEN];
	bool ignored;
	xcselect_manpaths *rv;

	bool developer_dir_valid = xcselect_get_developer_dir_path(path, MAXPATHLEN, &ignored, &ignored, &ignored);

	if (_xpc_runtime_is_app_sandboxed() && developer_dir_valid)
		return NULL;

	rv = (xcselect_manpaths*) malloc(sizeof(*rv));
	memset(rv, 0, sizeof(*rv));
	path_join(path_xcrun, MAXPATHLEN, path, "usr/lib/libxcrun.dylib");
	if (path_exists(path_xcrun) != 0) {
		void *open_xcrun = dlopen(path_xcrun, RTLD_LAZY);
		void (*xcrun_iter_manpaths)(const char* devpath, const char* sdkname, void (^)(const char*));
		if (!open_xcrun) {
			free(rv);
			fprintf(stderr, "%s: error: unable to load libxcrun (%s).\n", getprogname(), dlerror());
			return NULL;
		}

		xcrun_iter_manpaths = dlsym(open_xcrun, "xcrun_iter_manpaths");
		if (xcrun_iter_manpaths != NULL) {
			path[strlen(path)] = 0;
			xcrun_iter_manpaths(path, sdkname, ^(const char* path) {
				xcselect_manpaths_append(rv, path);
			});
		}

		dlclose(open_xcrun);
	} else {
		path_join(path_to_append, MAXPATHLEN, path, "usr/share/man");
		xcselect_manpaths_append(rv, path_to_append);

		path_join(path_to_append, MAXPATHLEN, path, "usr/llvm-gcc-4.2/share/man");
		xcselect_manpaths_append(rv, path_to_append);

		path_join(path_to_append, MAXPATHLEN, path, "Toolchains/XcodeDefault.xctoolchain/usr/share/man");
		xcselect_manpaths_append(rv, path_to_append);

	}
	return rv;
}
