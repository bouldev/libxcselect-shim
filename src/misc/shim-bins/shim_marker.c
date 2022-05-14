#include "shim_marker.h"

__attribute__((visibility("hidden")))
struct xcrun_shim_section_header shim_marker
	__attribute__((section("__DATA,__xcrun_shim")))
	= {0, 0};
