#include <xcselect.h>

int main(int argc, char *argv[]) {
	return xcselect_developer_dir_matches_path((strcmp(argv[1], "") == 0) ? NULL : argv[1], argv[2]);
}
