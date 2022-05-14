#include <xcselect.h>

extern char **_NSGetProgname(void);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmain-return-type"

void main(int argc, char* argv[])
{
	char *progname = *_NSGetProgname();
	char *tool_name = "clang";

	if (strcmp(progname, "cc") && strcmp(progname, "llvm-gcc")) {
		if (!strcmp(progname, "c++") || (tool_name = progname, !strcmp(progname, "llvm-g++")))
			tool_name = "clang++";
	}

	xcselect_invoke_xcrun(tool_name, argc - 1, argv + 1, false);
}
#pragma clang diagnostic pop
