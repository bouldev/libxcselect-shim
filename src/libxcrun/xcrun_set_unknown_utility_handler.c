#include <xcrun.h>

XC_EXPORT
void *xcrun_set_unknown_utility_handler(void *block_invoke)
{
	return Block_copy(block_invoke);
}
