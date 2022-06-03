#include <xcrun.h>
#include <errno.h>
#include <assert.h>
#include "xcinfocache.h"

XC_HIDDEN
xcinfocache *xcinfocache_create(const char *path)
{
	char tmpdir[MAXPATHLEN + 1];
	char *xcrun_db_clean;
	xcinfocache *xcrun_cache;
	char *xcrun_db = getenv("xcrun_db");

	if (xcrun_db) {
		xcrun_db_clean = path_clean_copy(xcrun_db);
		if (*xcrun_db_clean || *xcrun_db_clean == '/')
			goto skip_confstr;
		warning("environment variable 'xcrun_db' is not absolute, ignoring");
	}

	if (!confstr(_CS_DARWIN_USER_TEMP_DIR, tmpdir, MAXPATHLEN + 1) {
			warning("confstr() failed with code %i: couldn't get path of DARWIN_USER_TEMP_DIR; using /tmp instead", errno);
			strcpy(tmpdir, "/tmp");
	}
	char *tmpdir_clean = path_clean_copy(tmpdir);
	xcrun_db_clean = msprintf("%s/%s", tmpdir_clean, "xcrun_db");
skip_confstr:
	char *i;
	char developer_dir[MAXPATHLEN] = path_clean_copy(path);
	for (i = &developer_dir[strlen(developer_dir) - 1]; i > developer_dir && *i == '/'; --i)
		*i = '\0';

	if (!(developer_dir && developer_dir[0] == '/'))
		__assert_rtn("xcinfocache_create", "/Library/Caches/com.apple.xbs/Sources/xcrun/xcrun-59/src/xcinfocache.c", 304, "developer_dir && developer_dir[0] == '/'");
		// Orig code should be like assert("xcinfocache_create"); but shoudn't do this if we want same outputs.

	if (access(developer_dir, R_OK) != 0) {
		syserror("developer directory \'%s\' isn\'t accessible", developer_dir);
		return NULL;

	xcrun_cache = (xcinfocache *)calloc(1, sizeof(xcrun_cache)); // size should be 128
	xcrun_cache->cachefile = xcrun_db_clean;
	xcrun_cache->developer_dir = developer_dir;
	xcrun_cache->was_cltools = strendswith(developer_dir, "CommandLineTools");
	xcrun_cache->sdkroot = getenv("SDKROOT");

	char *toolchains_got = getenv("TOOLCHAINS");
	xcrun_cache->toolchains = toolchains_got ? toolchains_got : "";

	char *developer_usr_path_got = getenv("XCODE_DEVELOPER_USR_PATH");
	xcrun_cache->developer_usr_path = developer_usr_path_got ? path_clean_copy(developer_usr_path_got) : "";

	char *external_toolchain_dir_got = getenv("EXTERNAL_TOOLCHAINS_DIR");
	xcrun_cache->external_toolchain_dir = external_toolchain_dir_got ? path_clean_copy(external_toolchain_dir_got) : "";

	char *xcode_overriding_sdks_dir_got = getenv("XCODE_OVERRIDING_SDKS_DIRECTORY");
	xcrun_cache->xcode_overriding_sdks_dir = xcode_overriding_sdks_dir_got ? path_clean_copy(xcode_overriding_sdks_dir_got) : "";

	char *built_products_dirs_got = getenv("__XCODE_BUILT_PRODUCTS_DIR_PATHS");
	xcrun_cache->built_products_dirs = built_products_dirs_got ? path_clean_copy(built_products_dirs_got) : "";

	xcrun_cache->xcrun_config.nocache = util_get_bool_from_environment("xcrun_nocache");
	xcrun_cache->xcrun_config.verbose = util_get_bool_from_environment("xcrun_verbose");

	if (!*external_toolchain_dir_got && !*xcode_overriding_sdks_dir_got && !*built_products_dirs_got) {
		xcrun_cache->toolchain_dirs = xcrun_cache->developer_usr_path;
		xcrun_cache->sysroot_dirs = "";
return_incomplete:
		xcrun_cache->path_of_xcodebuild = msprintf("%s/usr/bin/xcodebuild", developer_dir);
		return xcrun_cache;
	}

	xcrun_cache->toolchain_dirs = msprintf("%s|%s|%s|%s", xcrun_cache->developer_usr_path, xcrun_cache->external_toolchain_dir, xcrun_cache->xcode_overriding_sdks_dir, xcrun_cache->built_products_dirs);
	xcrun_cache->sysroot_dirs = msprintf("%s|%s", xcrun_cache->xcode_overriding_sdks_dir, xcrun_cache->built_products_dirs);

	if (!*built_products_dirs_got) goto return_incomplete;
	char *path_of_xcodebuild = util_find_executable_in_paths_string("xcodebuild", built_products_dirs_got);
	xcrun_cache->path_of_xcodebuild = path_of_xcodebuild ? path_of_xcodebuild : "";
	if (!path_of_xcodebuild) goto return_incomplete;

	return xcrun_cache;
}

XC_HIDDEN
void xcinfocache_note_variables(xcinfocache* cache)
{
	note("PATH = '%s'", getenv("PATH"));
	note("SDKROOT = '%s'", cache->sdkroot);
	note("TOOLCHAINS = '%s'", cache->toolchains);
	note("DEVELOPER_DIR = '%s'", cache->developer_dir);
	note("XCODE_DEVELOPER_USR_PATH = '%s'", cache->developer_usr_path);
	char *external_toolchain_dir = cache->external_toolchain_dir;
	if (*external_toolchain_dir) note("EXTERNAL_TOOLCHAINS_DIR = '%s'", external_toolchain_dir);
	char *xcode_overriding_sdks_dir = cache->xcode_overriding_sdks_dir;
	if (*xcode_overriding_sdks_dir) note("XCODE_OVERRIDING_SDKS_DIRECTORY = '%s'", xcode_overriding_sdks_dir);
	char *built_products_dirs = cache->built_products_dirs;
	if (*built_products_dirs) note("__XCODE_BUILT_PRODUCTS_DIR_PATHS = '%s'", built_products_dirs);
	note("xcrun_db = '%s'", cache->cachefile);
}

#ifdef __has_include
#if __has_include(<mach-o/dyld_priv.h>)
#include <mach-o/dyld_priv.h>
#else
extern char **environ;
#endif
#endif

#include <sys/wait.h>

XC_HIDDEN
unsigned int xcinfocache_open_cache(xcinfocache *cache, bool debug_log, bool was_xcode_utility, bool check_license)
{
	cache->db = diskstrmap_open(cache->cachefile, cache->xcrun_config.verbose, debug_log);

	if (was_xcode_utility || cache->was_cltools) return 0;

	char *developer_dir = cache->developer_dir;
	char *in_app = strstr(developer_dir, ".app/");

	char *developer_dir_realpath;
	char *xcode_info_plist;
	if (!in_app && ((developer_dir_realpath = realpath(developer_dir, NULL)) == NULL || (developer_dir = developer_dir_realpath, (in_app == strstr(developer_dir_realpath, ".app/")) == 0))
	 || ((xcode_info_plist = msprintf("%s/Contents/Info.plist", path_clean_copy(strndup(developer_dir, in_app - developer_dir + 5)))) == 0)) {
		error("unable to find Xcode installation from active developer path \"%s\", use xcode-select to change", cache->developer_dir);
		return 1;
	}

	struct stat st;
	char *timestamp;
	if (stat(xcode_info_plist, &st) < 0) {
		syserror("couldn't stat Xcode's Info.plist");
	} else {
		timestamp = msprintf("%lld", st.st_mtimespec.tv_sec);
	}
	if (!stat("/Library/Developer/Toolchains", &st))
		timestamp = msprintf("%s|%lld", timestamp, st.st_mtimespec.tv_sec);

	char *homedir = getenv("HOME");
	char *xcodebuild_argv[128];
	if (homedir) {
		strlcpy(xcodebuild_argv[0], homedir, MAXPATHLEN);
		path_append(xcodebuild_argv[0], "Library/Developer/Toolchains");
		if (!stat(xcodebuild_argv[0], &st))
			timestamp = msprintf("%s|%lld", timestamp, st.st_mtimespec.tv_sec);
	}

	char *external_toolchain_dir = cache->external_toolchain_dir;
	if (*external_toolchain_dir && !stat(external_toolchain_dir, &st))
		timestamp = msprintf("%s|%lld", timestamp, st.st_mtimespec.tv_sec);

	if (!timestamp) return 1;

	char *strmap = msprintf("%s|%s|%s|<timestamp>", cache->developer_dir, (getenv("HOME")) ? getenv("HOME") : "<missing>", cache->external_toolchain_dir);
	struct database *db = cache->db;
	if (db) {
		if (!cache->xcrun_config.nocache) {
			char *lookup_strmap = diskstrmap_lookup(db, strmap);
			if (lookup_strmap) {
				if (strcmp(lookup_strmap, timestamp) == 0) return 0;
				if (cache->xcrun_config.verbose) note("reinitializing cache (Xcode signature changed)");
				cache->db = diskstrmap_open(cache->cachefile, true, debug_log);
			}
		}
	}

	char *path_of_xcodebuild = cache->path_of_xcodebuild;
	xcodebuild_argv[0] = path_of_xcodebuild;
	xcodebuild_argv[1] = "-license";
	xcodebuild_argv[2] = check_license ? "check" : "status";
	xcodebuild_argv[3] = NULL;
	while (posix_spawn(&st.st_dev, path_of_xcodebuild, NULL, NULL, xcodebuild_argv, environ)) {
		path_of_xcodebuild = cache->path_of_xcodebuild;
		if (errno != EINTR) {
			syserror("couldn't spawn '%s'", path_escape_shell_copy(cache->path_of_xcodebuild));
warn_and_exit:
			error("unable to locate xcodebuild, please make sure the path to the Xcode folder is set correctly!");
			error("You can set the path to the Xcode folder using /usr/bin/xcode-select -switch");
			return 71;
		}
	}

	int status;
	while (waitpid(st.st_dev, &status, 0) < 0) {
		if (errno != EINTR) {
			syserror("couldn't waitpid on pid %i", (unsigned int)st.st_dev);
			goto warn_and_exit;
		}
	}
	if ((status & 127) != 0) goto warn_and_exit;
	if (!status) if (cache->db) diskstrmap_insert(cache->db, strmap, timestamp);

	return (status & 127) ? status : 0;
}
