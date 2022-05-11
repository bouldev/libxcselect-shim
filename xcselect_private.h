#ifndef __XCSELECT_PRIVATE_H__
#define __XCSELECT_PRIVATE_H__

#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/sysctl.h>
#include <TargetConditionals.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

#define XC_EXPORT OS_EXPORT
#define XC_HIDDEN __attribute__((visibility("hidden")))
#define XCSELECT_VER "2384"
#define BUNDLE_PREFIX "com.apple."

#if !TARGET_OS_OSX && !defined(CF_PATH)
#define CF_PATH "/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation"
#elif !defined(CF_PATH)
#define CF_PATH "/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation"
#endif

#ifndef _PATH_XCSELECT_CONF
#define _PATH_XCSELECT_CONF "/etc/xcselect.conf"
#endif

typedef struct xcselect_manpaths {
	char **paths;
	uint32_t count;
} xcselect_manpaths;

__BEGIN_DECLS
/**
 * \brief Check if the given bundle ID is in developer tool list.
 *
 * \param bundle_id The bundle identifier of developer tool.
 *
 * \returns 1 on success, 0 on fail.
 */
XC_EXPORT
bool xcselect_bundle_is_developer_tool(char *bundle_id);

/**
 * \brief Check given path contained by or maches developer directory.
 *
 * \param developer_dir The path of developer directory, if no value was
 *                      given, current active developer directory path
 *                      will be used to compare.
 *
 * \param path The destinaton path for comparing with developer_dir.
 *
 * \returns 1 on success, 0 on fail.
 */
XC_EXPORT
bool xcselect_developer_dir_matches_path(const char * __nullable developer_dir,
				const char *path);

/**
 * \brief Get the path of current active developer directory.
 *
 * \param buffer The buffer stores the result developer directory path.
 *
 * \param buffer_size The buffer stores the size of resulting string.
 *
 * \param was_environment The buffer indicates that resulting path was
 *                        defined by environment variable (DEVELOPER_DIR).
 *
 * \param was_cltools The buffer indicates that resulting path was
 *                    CommandlineTools instance.
 *
 * \param was_default The buffer indicates that resulting path was
 *                    current active developer directory.
 *
 * \returns 1 on success, 0 on fail.
 */
XC_EXPORT
bool xcselect_get_developer_dir_path(char *buffer, int buffer_size,
				bool *was_environment, bool *was_cltools,
				bool *was_default);

/**
 * \brief Find developer contents from given path.
 *
 * \param path The path for searching developer contents.
 *
 * \param buffer The buffer stores the developer contents path.
 *
 * \param buffer_size The size limit of resulting string.
 *
 * \param was_cltools The buffer indicates that resulting path was
 *                    under CommandlineTools instance.
 *
 * \returns 1 on success, 0 on fail.
 */
XC_EXPORT
bool xcselect_find_developer_contents_from_path(char * __nonnull path, char *buffer,
				size_t buffer_size, bool *was_cltools);

/**
 * \brief Get the manpath to an SDK.
 *
 * \param sysroot The path for target SDK.
 *
 * \returns a xcselect_manpaths structure on success, otherwise
 *          returns nil.
 */
XC_EXPORT
xcselect_manpaths *xcselect_get_manpaths(char *sysroot);

/**
 * \brief Get current version of libxcselect.
 *
 * \returns a version string.
 */
XC_EXPORT
char *xcselect_get_version(void);

/**
 * \brief Deallocates the memory of given xcselect_manpaths
 *
 * \param xcp The target xcselect_manpaths.
 */
XC_EXPORT
void xcselect_manpaths_free(xcselect_manpaths *xcp);

/**
 * \brief Get the count of paths in given xcselect_manpaths.
 *
 * \param xcp The target xcselect_manpaths.
 *
 * \returns the count of paths in given xcselect_manpaths.
 */
XC_EXPORT
uint32_t xcselect_manpaths_get_num_paths(xcselect_manpaths *xcp);

/**
 * \brief Get the path of manpath with specific id in given xcselect_manpaths.
 *
 * \param xcp The target xcselect_manpaths.
 *
 * \param id The ID for specific manpath.
 *
 * \returns a string on found, otherwise returns nil.
 */
XC_EXPORT
const char *xcselect_manpaths_get_path(xcselect_manpaths *xcp, uint32_t id);

/**
 * \brief Trigger CommandlineTools install request.
 *
 * \param name The name of a command.
 *
 * \returns 1 when successfully triggered, otherwise returns 0.
 */
XC_EXPORT
bool xcselect_trigger_install_request(const char *tool_name);

/**
 * \brief Invoke xcrun to execute a command.
 *
 * \param tool_name The name of a command.
 *
 * \param argc Total count of arguments that being passed to.
 *
 * \param argv Arguments for tool_name.
 *
 * \param require_xcode Set to 0 to proceed. Set to 1 to trigger an error that
 *                      requires Xcode installation.
 */
XC_EXPORT
void xcselect_invoke_xcrun(char *tool_name, int argc, char * __nullable argv[], bool require_xcode);

#ifdef _XC_INTN
#ifndef BUNDLE_SEPARATOR
#define BUNDLE_SEPARATOR ";"
#endif
#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR ":"
#endif

XC_HIDDEN void path_append(char *path_dest, size_t length, const char *path_to_join);
XC_HIDDEN void path_join(char *path_dest, size_t length, const char *path_src, const char *path_extra);
XC_HIDDEN bool path_exists(char *path);
XC_HIDDEN bool path_contains_xcrun(const char *path);
XC_HIDDEN bool path_is_dir(const char *path);
XC_HIDDEN bool str_endswith(const char *str, const char *end);
XC_HIDDEN bool get_developer_dir_from_symlink(const char *path, char *buffer, int buffer_size, bool *status);
XC_HIDDEN void xcselect_manpaths_append(xcselect_manpaths* paths, const char* path);
XC_HIDDEN size_t sdks_at_path(const char *sdkdir, char * __nullable * __nonnull path, size_t length);
XC_HIDDEN void xcselect_invoke_xcrun_via_library(char *path_xcrun, char *tool_name, int argc, char * __nullable argv[], char *path_dev);
XC_HIDDEN void xcselect_invoke_xcrun_via_binary(char *path_xcrun, char * __nullable argv[], char *path_dev, bool was_environment);
XC_HIDDEN void *lazyCFSymbol(const char *symbol);
XC_HIDDEN bool is_path_xcrun_shim(const char *path);
XC_HIDDEN char *str_tolower(const char *str);

XC_HIDDEN int64_t xcselect_config_get_int64(const char *value);
XC_HIDDEN bool xcselect_config_get_bool(const char *value);
XC_HIDDEN void xcselect_free_config();
XC_HIDDEN char *xcselect_get_config_string(const char *key);
#endif

__END_DECLS

#pragma clang diagnostic pop

#endif
