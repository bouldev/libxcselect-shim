#include <xcselect.h>

struct xcrun_shim_section_header
{
	uint64_t message;
	uint64_t message2;
};

__attribute__((visibility("hidden")))
struct xcrun_shim_section_header shim_marker
	__attribute__((section("__DATA,__xcrun_shim")))
	= {0, 0};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmain-return-type"

// It is weird I know, but this has to be like that
void main(int argc, char** argv)
{
	const char *progname = getprogname();
	const char *tool_name, *xc_tool;

	if (*progname == '-')
		tool_name = progname + 1;
	else
		tool_name = progname;

	if (!strcasecmp(tool_name, "xcrun"))
		xc_tool = NULL;
	else
		xc_tool = tool_name;

	xcselect_invoke_xcrun(xc_tool, argc - 1, argv + 1,
			REQUIRE_XCODE ? true : false);
}
#pragma clang diagnostic pop
