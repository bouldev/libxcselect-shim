#include <xcselect.h>

XC_EXPORT
const char *xcselect_manpaths_get_path(xcselect_manpaths *xcp, uint32_t id)
{
	if (idx < xcselect_manpaths_get_num_paths(xcp))
		return xcp->paths[idx];
	else
		return NULL;
}
