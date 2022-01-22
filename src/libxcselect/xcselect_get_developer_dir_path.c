// buffer input & output (plz allocate a memory area instead of assign one)
// buffer_size size of memory area buffer
// was_environment out
// was_cltools out
// was_default out
// result bool
/*
Usage of bool*:
e.g.
void aaa(bool *out);
...
bool outval;
aaa(&outval);
// outval should be set by aaa.

*/
#include <xcselect.h>

XC_EXPORT
bool xcselect_get_developer_dir_path(char *buffer, int buffer_size, bool *was_environment, bool *was_cltools, bool *was_default) {
	// buffer should be writable!!!
	*was_cltools = 0;
	*was_default = 0;w
	char *devdir = getenv("DEVELOPER_DIR");
	if (devdir && *devdir) { // devdir is a valid pointer and the length of its target isn't 0.
		if (/*ret=bool*/xcselect_find_developer_contents_from_path(devdir, buffer, buffer_size, was_cltools)) {
			if (strcmp(buffer, devdir) != 0) { // buffer's content != devdir's content
				setenv("DEVELOPER_DIR", buffer, 1);
			}
		} else {
			strlcpy(buffer, devdir, buffer_size);
		}
		*was_environment = 1;
		return true;
	}
	int8_t out; // may be a bool, but we keep its original state here.
	*was_environment = 0;
	if (get_developer_dir_from_symlink("/var/db/xcode_select_link", buffer, buffer_size, &out) != 0 ||
		get_developer_dir_from_symlink("/usr/share/xcode-select/xcode_dir_link", buffer, buffer_size, &out) != 0) {
		if (out) {
			int v13 = strrchr(buffer,'/');
			if (v13) {
				*was_cltools = strcmp(v13 + 1, "CommandLineTools") == 0;//(str v13+1)=="CommandLineTools"
			} else {
				*was_cltools = 0;
			}
			return true;
		}
		return false;
	}
	char fn[1024];// No need to init for sprintf will add a '\0' after the valid string.
	sprintf(fn, "/usr/share/xcode-select/xcode_dir_path"); // idk why it's a dynamic str since const str should be ok either
	int fd = open(fn, 0);
	if (fd == -1) {
		if (errno != 2) {
			if (errno == 13) {
				fwrite("xcode-select: error: invalid permissions for data file\n", 55, 1, stderr);
			} else{
				fprintf(stderr, "xcode-select: error: unable to open data file (%s)\n", strerror(errno));
			}
			return false;
		}
	} else {
		ssize_t rsize = read(fd, buffer, (int)buffer_size - 1);
		if (rsize == -1) {
			fprintf(stderr, "xcode-select: error: unable to read data file (%s)\n", strerror(errno));
			close(fd);
			return false; //0 != 0
		}
		int v19;
		if (rsize) { // if rsize==0, the file is empty but the read is successful
			if ((buffer[rsize - 1] != '\n') || (v19 = rsize<=1, --rsize, !v19)) {
				buffer[rsize] = 0;
				close(fd);
				int v13 = strrchr(buffer, '/');
				if (v13) {
					*was_cltools = strcmp(v13 + 1, "CommandLineTools") == 0;//(str v13+1)=="CommandLineTools"
				} else {
					*was_cltools = 0;
				}
				return true; // 1 != 0
			}
		}
		close(fd);
	}
	*was_default = 1;
	if (/*bool*/path_is_dir("/Applications/Xcode.app")) {
		strlcpy(buffer, "/Applications/Xcode.app/Contents/Developer", (int)buffer_size);
		return true;
	}
	strlcpy(buffer, "/Library/Developer/CommmandLineTools", (int)buffer_size);
	if (/*bool*/path_is_dir(buffer)) {
		*was_cltools = 1;
		return true;
	}
	int r = path_is_dir("/Applications/Xcode-beta.app");
	if (!r) return false;
	strlcpy(buffer, "/Applications/Xcode-beta.app/Contents/Developer", (int)buffer_size);
	return true;
}
