#include <xcselect.h>

XC_EXPORT
const char *xcselect_manpaths_get_path(xcselect_manpaths *xcp, uint32_t id)
{
	if (id < xcselect_manpaths_get_num_paths(xcp))
		return xcp->paths[id];
	else
		return NULL;
}
