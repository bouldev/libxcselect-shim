#include <xcselect.h>

int main(int argc, char *argv[]) {
	return xcselect_bundle_is_developer_tool((argv[1]) ? argv[1] : "");
}
