#include <xcrun.h>

XC_HIDDEN
int xcrun_main2(char *utility, int argc, char *argv[], char *path_dev)
{
	char *get_argv;
	bool xcrun_find = util_get_bool_from_environment("xcrun_find");
	bool xcrun_log = util_get_bool_from_environment("xcrun_log");
	if (!utility) {
		if (argc == 0) {
PRINT_USAGE:
			PrintUsageAndExit(64);
PRINT_VERSION:
			fprintf(stdout, "xcrun version %s.\n", XCRUN_VER);
			exit(0);
		}
		do {
			get_argv = *argv;
			if (*get_argv != '-') {
				if (xcrun_find) {
					
}

XC_EXPORT
int xcrun_main(char *utility, int argc, char *argv[], char *path_dev)
{
	exit(xcrun_main2(utility, argc, argv, path_dev));
}
