#include <xcselect.h>

extern char **_NSGetProgname(void);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmain-return-type"

void main(int argc, char *argv[])
{
	xcselect_invoke_xcrun(*_NSGetProgname(), argc - 1, argv + 1, true);
}
#pragma clang diagnostic pop
