#include <xcselect.h>
#include <dispatch/dispatch.h>

#ifdef __has_include
#if __has_include(<os/assumes.h>)
#include <os/assumes.h>
#else
extern void _os_assumes_log(uint64_t code);
#endif
#endif

static struct {
	dispatch_once_t onceToken;
	char product[64];
	long version;
} host_version;

XC_EXPORT
errno_t xcselect_host_sdk_path(xcselect_host_sdk_policy_t sdk_policy,
                               char * __nullable * __nonnull path)
{
	errno_t result = EINVAL;
	if (!path || sdk_policy - XCSELECT_HOST_SDK_POLICY_MATCHING_PREFERRED > XCSELECT_HOST_SDK_POLICY_MATCHING_ONLY)
		return result;

	char *path_return, *path_idk_wtf, *path_idk_wtf_2, *path_idk_wtf_3, *path_return_2;
	char **path_at = (char **)calloc(0x40, 8);
	char path_dev[MAXPATHLEN], *sdkroot;
	bool was_environment, was_cltools, was_default;
	if (host_version.onceToken != -1) {
		dispatch_once(&host_version.onceToken, ^{
			size_t sysctl_size = sizeof(host_version.product); // Not greater than 20 normally
			char host_ver[64];
			if (sysctlbyname("hw.product", host_version.product, &sysctl_size, NULL, 0) != 0)
				sysctlbyname("hw.machine", host_version.product, &sysctl_size, NULL, 0);
			if (sysctlbyname("kern.osproductversion", host_ver, &sysctl_size, NULL, 0) == -1)
				_os_assumes_log((uint64_t)errno);
			host_version.version = (long)atoi(host_ver);
		});
	}

	errno_t sdk_status = 0;
	if (!getenv("SDKROOT") && strstr(host_version.product, "Mac") != NULL) {
		if (xcselect_get_developer_dir_path(path_dev, MAXPATHLEN, &was_environment, &was_cltools, &was_default) && !was_cltools) {
			asprintf(&sdkroot, "%s/Platforms/MacOSX.platform/Developer/SDKs", path_dev);
			sdk_status = sdks_at_path(sdkroot, path_at, 64);
			free(sdkroot);
		}
		long long_idk_wtf = sdks_at_path("/Library/Developer/CommandLineTools/SDKs", path_at + sdk_status, 0x40 - sdk_status);
		long_idk_wtf = long_idk_wtf + sdk_status;
		if (long_idk_wtf == 0) {
			path_idk_wtf = NULL;
			path_return = NULL;
		} else {
			int cmp_result;
			long sdk_version;
			unsigned int uint_idk_wtf, uint_idk_wtf_2, uint_idk_wtf_3;
			path_idk_wtf = NULL;
			path_idk_wtf_2 = NULL;
			char **path_at_2 = path_at;
			uint_idk_wtf_3 = 0;
			do {
				path_idk_wtf_3 = *path_at_2;
				path_return = strrchr(path_idk_wtf_3, '/');
				cmp_result = strncmp("MacOSX10.", path_return + 1, 9);
				if (!cmp_result) {
					path_return = strdup(path_return + 10);
					path_return_2 = strchr(path_return, '.');
					cmp_result = strcmp(path_return_2, ".sdk");
					if (cmp_result == 0) {
						*path_return_2 = '\0';
						sdk_version = strtol(path_return, NULL, 10);
						uint_idk_wtf = (unsigned int)sdk_version;
					} else {
						uint_idk_wtf = 0;
					}
					free(path_return);
				} else {
					uint_idk_wtf = 0;
				}
				uint_idk_wtf_2 = uint_idk_wtf;
				path_return = path_idk_wtf_3;
				if (path_idk_wtf != NULL && uint_idk_wtf <= uint_idk_wtf_3) {
					uint_idk_wtf_2 = uint_idk_wtf_3;
					path_return = path_idk_wtf;
				}
				path_idk_wtf = path_return;
				path_return = path_idk_wtf_3;
				if (uint_idk_wtf != (unsigned int)host_version.version) {
					path_return = NULL;
				}
				if (path_idk_wtf_2 != NULL) {
					path_return = path_idk_wtf_2;
				}
				if ((path_idk_wtf_3 != path_idk_wtf) && (path_idk_wtf_3 != path_return)) {
					free(path_idk_wtf_3);
				}
				path_at_2 = path_at_2 + 1;
				long_idk_wtf = long_idk_wtf - 1;
				path_idk_wtf_2 = path_return;
				uint_idk_wtf_3 = uint_idk_wtf_2;
			} while (long_idk_wtf != 0);
		}
		free(path_at);
		path_idk_wtf_2 = path_idk_wtf;
		if (sdk_policy == XCSELECT_HOST_SDK_POLICY_MATCHING_PREFERRED) {
			if (path_return != NULL) {
				path_idk_wtf_2 = path_return;
			}
wrap:
			if (path_idk_wtf_2 != path_return) {
				free(path_return);
				path_return = path_idk_wtf_2;
			}
		} else if (sdk_policy != XCSELECT_HOST_SDK_POLICY_MATCHING_ONLY) {
			if (sdk_policy != XCSELECT_HOST_SDK_POLICY_LATEST) {
				path_idk_wtf_2 = NULL;
			}
			goto wrap;
		}
		if (path_return != path_idk_wtf) {
			free(path_idk_wtf);
		}
		*path = path_return;
		result = (errno_t)(path_return == NULL) << 1;
	} else if (getenv("SDKROOT") != NULL) {
		*path = getenv("SDKROOT");
		result = 0;
	} else {
		*path = "/";
		result = 0;
	}
	return result;
}
