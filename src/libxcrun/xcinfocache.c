#include <xcrun.h>
#include <errno.h>
#include <assert.h>

XC_HIDDEN
char **xcinfocache_create(char *path)
{
	char *darwin_user_temp_dir;
	char *xcrun_db = getenv("xcrun_db");
	if (((!xcrun_db) || (xcrun_db = path_clean_copy(xcrun_db), !xcrun_db)) || (*xcrun_db == '\0') || (*xcrun_db != '/')) {
		if (*xcrun_db != '/') warning("environment variable \'xcrun_db\' is not absolute, ignoring");

		if (confstr(0x10001, &darwin_user_temp_dir, 0x401) == 0) {
			warning("confstr() failed with code %i: couldn\'t get path of DARWIN_USER_TEMP_DIR; using /tmp instead", errno);
			darwin_user_temp_dir = "/tmp";
		}
		xcrun_db = msprintf("%s/%s", path_clean_copy(&darwin_user_temp_dir), "xcrun_db");
	}

	char *developer_dir = path_clean_copy(path);
	char *developer_dir_remove_slash;
	for (developer_dir_remove_slash = developer_dir + (strlen(developer_dir) - 1); (developer_dir < developer_dir_remove_slash && (*developer_dir_remove_slash == '/')); developer_dir_remove_slash = developer_dir_remove_slash - 1) {
		*developer_dir_remove_slash = '\0';
	}
	if (!developer_dir || (*developer_dir != '/')) {
		__assert_rtn("xcinfocache_create", "/Library/Caches/com.apple.xbs/Sources/xcrun/xcrun-59/src/xcinfocache.c", 304, "developer_dir && developer_dir[0] == \'/\'");
	}

	char **xcrun_env;
	if (access(developer_dir, R_OK) == 0) {
		xcrun_env = calloc(1, 128);
		*xcrun_env = xcrun_db;
		*(bool *)(xcrun_env + 2) = util_get_bool_from_environment("xcrun_verbose");
		xcrun_env[3] = developer_dir;
		*(bool *)(xcrun_env + 4) = strendswith(developer_dir, "CommandLineTools");
		xcrun_env[6] = getenv("SDKROOT");
		xcrun_env[8] = (getenv("TOOLCHAINS")) ? getenv("TOOLCHAINS") : "";
		xcrun_env[9] = (getenv("XCODE_DEVELOPER_USR_PATH")) ? path_clean_copy(getenv("XCODE_DEVELOPER_USR_PATH")) : "";
		xcrun_env[10] = (getenv("EXTERNAL_TOOLCHAINS_DIR")) ? path_clean_copy(getenv("EXTERNAL_TOOLCHAINS_DIR")) : "";
		xcrun_env[11] = (getenv("XCODE_OVERRIDING_SDKS_DIRECTORY")) ? path_clean_copy(getenv("XCODE_OVERRIDING_SDKS_DIRECTORY")) : "";
		xcrun_env[12] = (getenv("__XCODE_BUILT_PRODUCTS_DIR_PATHS")) ? path_clean_copy(getenv("__XCODE_BUILT_PRODUCTS_DIR_PATHS")) : "";
		*(bool *)((long)xcrun_env + 18) = util_get_bool_from_environment("xcrun_nocache");
		if ((*xcrun_env[10] == '\0') && (*xcrun_env[11] == '\0') && (*xcrun_env[12] == '\0')) {
			xcrun_env[14] = xcrun_env[9];
			xcrun_env[15] = "";
		} else {
			xcrun_env[14] = msprintf("%s|%s|%s|%s", xcrun_env[9], xcrun_env[10], xcrun_env[11], xcrun_env[12]);
			xcrun_env[15] = msprintf("%s|%s", xcrun_env[11], xcrun_env[12]);
			if (*xcrun_env[12] != '\0') {
				xcrun_env[5] = util_find_executable_in_paths_string("xcodebuild", xcrun_env[12]);
				if (xcrun_env[5]) return xcrun_env;
			}
		}
		xcrun_env[5] = msprintf("%s/usr/bin/xcodebuild", developer_dir);
	} else {
		syserror("developer directory \'%s\' isn\'t accessible", developer_dir);
	}

	return xcrun_env;
}

/* I give up, that's too shity to operate with manpaths
XC_HIDDEN
uint32_t xcinfocache_open_cache(char *xcinfocache, bool write_log, bool require_xcode, bool)
{
	xcinfocache[1] = diskstrmap_open(*xcinfocache, xcinfocache[17], write_log);
	if (require_xcode || xcinfocache[4]) return 0;

	char *developer_dir = (char *)xcinfocache[3];
	char *app_dir = strstr(developer_dir,".app/");
	if (!app_dir) {
		if (app_dir = realpath(developer_dir, 0), !app_dir)
*/
