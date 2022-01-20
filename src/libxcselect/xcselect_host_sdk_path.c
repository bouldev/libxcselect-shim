#include <xcselect.h>

static struct {
	dispatch_once_t onceToken;
	char *product;
	long version;
} host_version;

XC_EXPORT
errno_t xcselect_host_sdk_path(xcselect_host_sdk_policy_t sdk_policy,
                               char * __nullable * __nonnull path)
{
	errno_t status = EINVAL;
	if (!path || sdk_policy - 1 > 2)
		return status;

	char *path_return, *path_idk_wtf, *path_idk_wtf_2, *path_idk_wtf_3, *path_return_2;
	char **path_at = (char **)calloc(0x40, 8);
	char path_dev[MAXPATHLEN], sdkroot[MAXPATHLEN];
	bool was_environment, was_cltools, was_default;
	if (host_version.onceToken != -1) {
		dispatch_once(&host_version.onceToken, ^{
			size_t sysctl_size;
			if (sysctlbyname("hw.product", host_version.product, &sysctl_size, NULL, 0) != 0)
				sysctlbyname("hw.machine", host_version.product, &sysctl_size, NULL, 0);
			if (sysctlbyname("kern.osproductversion", host_version.version, &sysctl_size, NULL, 0) == -1)
				_os_assumes_log((uint64_t)errno);
		});
		status = 0;
		if (getenv("SDKROOT") == NULL && strstr(host_version.product, "Mac") != NULL) {
			if ((xcselect_get_developer_dir_path(path_dev, MAXPATHLEN, &was_environment, &was_cltools, &was_default) != 0) && ((was_default | was_cltools) == 0)) {
				asprintf(&sdkroot, "%s/Platforms/MacOSX.platform/Developer/SDKs", path_dev);
				status = sdks_at_path(sdkroot, path_at, 64);
				free(sdkroot);
			}
			long long_idk_wtf = sdks_at_path("/Library/Developer/CommandLineTools/SDKs", path_at + status, 0x40 - status) + status;
			if (long_idk_wtf == 0) {
				path_idk_wtf = NULL;
				path_return = NULL;
			} else {
				int cmp_result;
				unsigned int uint_idk_wtf, uint_idk_wtf_2, uint_idk_wtf_3;
				path_idk_wtf = NULL;
				path_idk_wtf_2 = NULL;
				char **path_at_2 = path_at;
				uint_idk_wtf_3 = 0;
				do {
					path_idk_wtf_3 = *path_at_2;
					path_return = strrchr(path_idk_wtf_3, 0x2f);
					cmp_result = strncmp("MacOSX10.", path_return + 1, 9);
					if (cmp_result == 0) {
						path_return = strdup(path_return + 10);
						path_return_2 = strchr(path_return, 0x2e);
						cmp_result = strcmp(path_return_2, ".sdk");
						if (cmp_result == 0) {
							*path_return_2 = '\0';
							status = strtol(path_return, NULL, 10);
							uint_idk_wtf = (unsigned int)status;
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
					if (uint_idk_wtf != host_version.version) {
						path_return = NULL;
					}
					if (path_idk_wtf_2 != NULL) {
						path_return = path_idk_wtf_2;
					}
					if ((path_idk_wtf_3 != path_idk_wtf) && (path_idk_wtf_3 != path_return)) {
						free(path_idk_wtf_3);
					}
					path_at_2 = path_at_2 + 1;
					long_idk_wtf = long_idk_wtf + -1;
					path_idk_wtf_2 = path_return;
					uint_idk_wtf_3 = uint_idk_wtf_2;
				} while (long_idk_wtf != 0);
			}
			free(path_at);
			path_idk_wtf_2 = path_idk_wtf;
			if (sdk_policy == 1) {
				if (path_return != NULL) {
					path_idk_wtf_2 = path_return;
				}
				if (path_idk_wtf_2 != path_return) {
					free(path_return);
					path_return = path_idk_wtf_2;
				}
			} else if (sdk_policy == 2) {
				if (sdk_policy != 3) {
					path_idk_wtf_2 = NULL;
				}
				if (path_idk_wtf_2 != path_return) {
					free(path_return);
					path_return = path_idk_wtf_2;
				}
			}
			if (path_return != path_idk_wtf) {
				free(path_idk_wtf);
			}
			*path = path_return;
			status = (errno_t)(path_return == NULL) << 1;
		} else if (getenv("SDKROOT") != NULL) {
			*path = getenv("SDKROOT");
			status = 0;
		} else {
			*path = "/";
			status = 0;
		}
	}
	return status;
}
