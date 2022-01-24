#include <xcselect.h>

extern int _xpc_runtime_is_app_sandboxed(void);

XC_EXPORT
void xcselect_invoke_xcrun(char *tool_name, int argc, char *argv[], bool require_xcode) {
	if (_xpc_runtime_is_app_sandboxed()) {
		fwrite("xcrun: error: cannot be used within an App Sandbox.\n", 52, 1, stderr);
		exit(1);
	}
	bool was_environment, was_cltools, was_default;
	char path_dev[MAXPATHLEN], path_xcrun_lib[MAXPATHLEN], path_xcrun_bin[MAXPATHLEN];
	if (xcselect_get_developer_dir_path(path_dev, MAXPATHLEN, &was_environment, &was_cltools, &was_default) != 0) {
		if (was_cltools && require_xcode) {
			fprintf(stderr,"xcode-select: error: tool '%s' requires Xcode, but active developer directory '%s' is a command line tools instance\n", tool_name, path_dev);
		} else {
			if (tool_name) unsetenv("xcrun_log");
			path_join(path_xcrun_lib, MAXPATHLEN, path_dev, "usr/lib/libxcrun.dylib");
			if (path_exists(path_xcrun_lib))xcselect_invoke_xcrun_via_library(path_xcrun_lib, tool_name, argc, argv, path_dev);
			path_join(path_xcrun_bin, MAXPATHLEN, path_dev, "usr/bin/xcrun");
			if (!is_path_xcrun_shim(path_xcrun_bin)) {
				int xc_argc;
				if (tool_name) {
					xc_argc = argc + 1;
				} else {
					xc_argc = argc;
				}
				char **xc_argv = malloc(((long)(xc_argc + 2) << 32) >> 29);
				*xc_argv = path_xcrun_bin; // It assigns a char * to the target of the pointer
				          // xc_argv so we can infer that xc_argv's type is char**.
				if (tool_name) {
					xc_argv[1] = tool_name; // As xc_argv is a char** pointer, its element [1] may be also a char *.
						   // So we set tool_name to char*.
				}
				if (argc) {
					int v19 = argc;
					int v20 = (v19 != 0) + 1; // 1 or 2
					do {
						xc_argv[v20++] = *argv++;
						// Things assigned to xc_argv is char*, assigning *argv infers that argv is char**.
						--v19;
					} while (v19); // v19!=0
				}
				xc_argv[xc_argc + 1] = 0;
				xcselect_invoke_xcrun_via_binary(path_xcrun_bin, xc_argv, path_dev, was_environment);
			}
			fprintf(stderr, "xcode-select: error: malformed developer path (\"%s\")\n", path_dev);
		}
	} else {
		if (!tool_name || (path_join(path_xcrun_lib, MAXPATHLEN, "/usr/libexec/DeveloperTools", tool_name), !path_exists(path_xcrun_lib))) {
			if (was_default) {
				const char *progname;
				if (tool_name) {
					progname = getprogname();
				} else {
					progname = "xcrun";
				}

				if (xcselect_trigger_install_request(progname)) {
					fwrite("xcode-select: note: no developer tools were found at '/Applications/Xcode.app', requesting install. Choose an option in the dialog to download the command line developer tools.\n", 177, 1, stderr);
				} else {
					fwrite("xcode-select: error: no developer tools were found at '/Applications/Xcode.app', and no install could be requested (perhaps no UI is present), please install manually from 'developer.apple.com'.\n", 195, 1, stderr);
				}
				exit(1);
			} else {
				fwrite("xcrun: error: unable to get active developer path\nUse `sudo xcode-select --switch path/to/Xcode.app` to specify the Xcode that you wish to use for command line developer tools, or use `xcode-select --install` to install the standalone command line developer tools.\nSee `man xcode-select` for more details.\n", 306, 1, stderr);
			}
		} else {
			char **v11 = malloc(((long)(argc + 2) << 32) >> 29);
			*v11 = path_xcrun_lib;
			if (argc) {
				memcpy(v11 + 1, argv, 8 * argc);
			}
			v11[argc + 1] = 0;
			execv(path_xcrun_lib, v11);
			fprintf(stderr, "xcrun: error: unable to exec %s (%s).\n", path_xcrun_lib, strerror(errno));
		}
	}
	exit(1);
}
