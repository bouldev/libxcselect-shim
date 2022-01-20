#include <xcselect.h>

XC_EXPORT
uint32_t xcselect_manpaths_get_num_paths(xcselect_manpaths *xcp)
{
	return xcp->count;
}
