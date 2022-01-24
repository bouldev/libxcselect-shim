#include <xcselect.h>

XC_EXPORT
bool xcselect_trigger_install_request(const char *tool_name)
{
	// Never return false except CoreFoundation missing or not callable
	return true;
}
