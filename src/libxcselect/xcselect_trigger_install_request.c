#include <xcselect.h>

XC_EXPORT
bool xcselect_trigger_install_request(char *name)
{
	// Never return false except CoreFoundation missing or not callable
	return true;
}
