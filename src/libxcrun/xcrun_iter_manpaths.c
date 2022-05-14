#include <xcrun.h>

XC_EXPORT
void xcrun_iter_manpaths(char* path_dev, const char* sdkname, const char *path)
{
/* this func does not return, so lets stub it
	char path_toolchain[MAXPATHLEN];
	char **xc_cache_path_dev = xcinfocache_create(path_dev);
	if (!xc_cache_path_dev) exit(74);

	if (sdkname) {
		if (xc_cache_path_dev[8]) {
			__assert_rtn("xcinfocache_set_sdkroot", "/Library/Caches/com.apple.xbs/Sources/xcrun/xcrun-59/src/xcinfocache.c", 398, "!cache->db && \"cannot set reset cache value after open!\"");
		}
		*(char **)(xc_cache_path_dev + 0x30) = strdup(sdkname);
		*(bool *)(xc_cache_path_dev + 0x38) = 1;
	}
	if (xcinfocache_open_cache(xc_cache_path_dev, 0, 0, 0) == 0) {
		xcinfocache_resolve_sdkroot(xc_cache_path_dev, 0, 0);
		xcrun_call_manpath_iter_with_prefix((xc_cache_path_dev + '0'), path);
		xcrun_call_manpath_iter_with_prefix((xc_cache_path_dev + 'h'), path);
	}
	xcinfocache_free(xc_cache_path_dev);
	xcrun_call_manpath_iter_with_prefix(path_dev, path);
	strlcpy(path_toolchain, path_dev, MAXPATHLEN);
	path_append(path_toolchain, "Toolchains/XcodeDefault.xctoolchain");
	xcrun_call_manpath_iter_with_prefix(path_toolchain, path);
*/
}
