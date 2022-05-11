#include <xcselect.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dispatch/dispatch.h>

XC_HIDDEN
void path_append(char *path_dest, size_t length, const char *path_to_join)
{
	size_t origLength = strlen(path_dest);
	if ((origLength != 0) && (path_dest[origLength - 1] != '/' && origLength + 1 < length)) {
		path_dest[origLength] = '/';
		path_dest[origLength + 1] = '\0';
	}
	strlcat(path_dest, path_to_join, length);
}

XC_HIDDEN
void path_join(char *path_dest, size_t length, const char *path_src, const char *path_extra)
{
	strlcpy(path_dest, path_src, length);
	path_append(path_dest, length, path_extra);
}

XC_HIDDEN
bool path_exists(char *path)
{
	struct stat st;
	return !stat(path, &st);
}

XC_HIDDEN
bool path_contains_xcrun(const char *path)
{
	char buffer[MAXPATHLEN];
	struct stat st;
	path_join(buffer, MAXPATHLEN, path, "usr/lib/libxcrun.dylib");
	if (!stat(buffer, &st) && S_ISREG(st.st_mode)) {
		return true;
	}
	path_join(buffer, MAXPATHLEN, path, "usr/bin/xcrun");
	if (stat(buffer, &st)) return false;
	if (S_ISREG(st.st_mode)) {
		return (st.st_mode >> 6) & 1;
	}
	return false;
}

XC_HIDDEN
bool path_is_dir(const char *path)
{
	struct stat st;
	return !stat(path, &st) && S_ISDIR(st.st_mode);
}

XC_HIDDEN
bool str_endswith(const char *str, const char *end)
{
	size_t str_size = strlen(str);
	size_t end_size = strlen(end);
	if ((str_size < end_size) || (memcmp(str + (str_size - end_size), end, end_size + 1) != 0)) {
		return false;
	}
	return true;
}

XC_HIDDEN
char *str_tolower(const char *str)
{
	char *s;
	strcpy(s, str);
	char *d = (char *)malloc(strlen(s));
	while (*s)
	{
		*d = tolower(*s);
		d++;
		s++;
	}
	return d;
}

XC_HIDDEN
bool get_developer_dir_from_symlink(const char *path, char *buffer, int buffer_size, bool *status)
{
	// status 0: exists but err, 1: exists and no error, NULL: not exist
	ssize_t read_stat = readlink(path, buffer, (long)(buffer_size - 1));
	if (read_stat < 1) {
		if (errno != ENOENT) {
			if (errno == EACCES) {
				fprintf(stderr, "xcode-select: error: invalid permissions for data link at \'%s\'\n", path);
			} else {
				fprintf(stderr, "xcode-select: error: unable to read data link at \'%s\', expected symbolic link (%s)\n", path, strerror(errno));
			}
			*status = false;
		} else {
			return false;
		}
	} else {
		buffer[read_stat] = '\0';
		*status = true;
	}
	return true;
}

XC_HIDDEN
void xcselect_manpaths_append(xcselect_manpaths* paths, const char* path)
{
	paths->count++;
	paths->paths = realloc(paths->paths, sizeof(char*) * paths->count);
	paths->paths[paths->count - 1] = strdup(path);
}

XC_HIDDEN
size_t sdks_at_path(const char *sdkdir, char * __nullable * __nonnull path, size_t length)
{
	DIR *try_open = opendir(sdkdir);
	DIR *try_read_open;
	struct dirent *try_read;
	struct stat st;
	size_t status;
	int asprintf_result;
	char *path_sys_cdefs_h;

	if (!try_open) return 0;
	try_read_open = try_open;

	try_read = readdir(try_open);
	status = 0;
	if (try_read && length) {
		status = 0;
		do {
			asprintf_result = asprintf(&path_sys_cdefs_h, "%s/%s/usr/include/sys/cdefs.h", sdkdir, try_read->d_name);
			if (!stat(path_sys_cdefs_h, &st)) {
				path_sys_cdefs_h[asprintf_result - 24] = 0;
				path[status++] = path_sys_cdefs_h;
			} else {
				free(path_sys_cdefs_h);
			}
			try_read = readdir(try_read_open);
		} while (status < length && try_read);
	}
	closedir(try_read_open);
	return status;
}

XC_HIDDEN
void xcselect_invoke_xcrun_via_library(char *path_xcrun, char *tool_name, int argc, char *argv[], char *path_dev)
{
	extern char **_NSGetProgname(void);
	void *open_xcrun = dlopen(path_xcrun, RTLD_LOCAL | RTLD_LAZY);
	if (!open_xcrun) {
		fprintf(stderr, "xcrun: error: unable to load libxcrun (%s).\n", dlerror());
	} else {
		int (*xcrun_main)(char *tool_name, int argc, char *argv[], char *devidr);
		void (*xcrun_set_unknown_utility_handler)(const void *block_invoke);
		char **progname = _NSGetProgname();
		xcrun_main = dlsym(open_xcrun, "xcrun_main");
		xcrun_set_unknown_utility_handler = dlsym(open_xcrun, "xcrun_set_unknown_utility_handler");
		if (strcmp(*progname, "xcrun") != 0 && xcrun_set_unknown_utility_handler != NULL) {
			int block_return;
			xcrun_set_unknown_utility_handler(^(int block_return) {
				block_return = xcselect_trigger_install_request(tool_name);
				if (block_return == 0) {
					fprintf(stderr, "xcode-select: Failed to locate \'%s\', and no install could be requested (perhaps no UI is present). Please install manually from \'developer.apple.com\'.\n", tool_name);
				} else {
					fprintf(stderr, "xcode-select: Failed to locate \'%s\', requesting installation of command line developer tools.\n", tool_name);
				}
				return block_return;
			});
		}
		if (xcrun_main(tool_name, argc, argv, path_dev) != 0) {
			fwrite("xcrun: error: unexpected exit from xcrun_main", 0x2d, 1, stderr);
			exit(1);
		}
		fprintf(stderr, "xcrun: error: unable to resolve xcrun_main (%s).\n", dlerror());
	}
}

XC_HIDDEN
void xcselect_invoke_xcrun_via_binary(char *path_xcrun, char *argv[], char *path_dev, bool was_environment)
{
	execv(path_xcrun, argv);
	if (errno == ENOENT) { // path_xcrun not exist
		char *dev_dir_type, *dev_dir_error;
		struct stat st;
		int path_dev_stat = stat(path_dev, &st);
		if ((path_dev_stat == 0) && ((st.st_mode & S_IFMT) == S_IFDIR)) { // devdir exists
			dev_dir_type = "active developer";
			if (was_environment) {
				dev_dir_type = "DEVELOPER_DIR";
			}
			fprintf(stderr, "xcrun: error: invalid %s path (%s), missing xcrun at: %s\n", dev_dir_type, path_dev, path_xcrun);
		} else { // devdir invalid
			if (path_dev_stat == 0) {
				dev_dir_error = "is invalid";
			} else {
				dev_dir_error = "does not exist";
			}
			fprintf(stderr, "xcrun: error: active developer path (\"%s\") %s\nUse `sudo xcode-select --switch path/to/Xcode.app` to specify the Xcode that you wish to use for command line developer tools, or use `xcode-select --install` to install the standalone command line developer tools.\nSee `man xcode-select` for more details.\n", path_dev, dev_dir_error);
		}
	} else { // path_xcrun exists but not executable
		fprintf(stderr, "xcrun: error: unable to exec Xcode native xcrun (%s).\n", strerror(errno));
	}
	exit(1);
}

XC_HIDDEN
void *lazyCFSymbol(const char *symbol)
{
	static struct {
		dispatch_once_t onceToken;
		void *lib;
	} _lazyCFSymbol;
	if (_lazyCFSymbol.onceToken != -1) {
		dispatch_once(&_lazyCFSymbol.onceToken, ^{
			_lazyCFSymbol.lib = dlopen(CF_PATH, RTLD_LAZY);
		});
	}
	if (_lazyCFSymbol.lib != NULL) {
		return dlsym(_lazyCFSymbol.lib, symbol);
	}
	return NULL;
}

XC_HIDDEN
bool is_path_xcrun_shim(const char *path)
{
	FILE *file;
	file = fopen(path, "rb");
	if (file) {
		int v14;
		if(fread(&v14,4,1,file)==1) {
			int seekval=0;
			if(v14==-1095041334) {
				int64_t tmp;
				char v15[78];
				if(fseek(file,0,0)==-1||fread(&tmp,8,1,file)!=1||fread(v15,20,1,file)!=1) {
					fclose(file);
					return false;
				}
				register unsigned int *ptr=(unsigned int*)(v15+8);
				//*(int32_t *)&seekval=htonl(*ptr);
				seekval=htonl(*ptr);
			}else{
				seekval=0;
			}
			int v11[4];
			if(fseek(file,seekval,0)!=-1&&fread(v11,32,1,file)==1&&v11[0]==-17958193) {
				register int v12;
				__asm__ __volatile__("ldr %w0, [sp, #0x10]":"=r"(v12));
				int v5=v12;
				if(v12) {
					int v6=0;
					while(1) {
						int v7=ftell(file);
						int v13[3];
						char v16[72];
						if(fread(v13,8,1,file)!=1)
							break;
						if(v13[0]==25) {
							if(fseek(file,v7,0)==-1||fread(v16,72,1,file)!=1) {
								break;
							}
							register int32_t *v17;
							__asm__ __volatile__("ldr %w0, [sp, #0x88]":"=r"(v17));
							if(!(v17[0]^0x41445F5F|*(int32_t *)((char*)v17+3)^0x415441)) {
								register int v18;
								__asm__ __volatile__("ldr %w0, [sp, #0xc0]":"=r"(v18));
								int v8=v18;
								if(v18) {
									char buf[128];
									while(fread(buf,0x50,1,file)==1) {
										if(!((*(int64_t*)buf)^0x5F6E757263785F5FLL|(*(int64_t*)(buf+5))^0x6D6968735F6E75LL)) {
											fclose(file);
											return true;
										}
										if(!--v8) {
											break;
										}
									}
								}
							}
						}
						if(fseek(file,v7+(unsigned int)v13[1],0)!=-1&&++v6!=v5)
							continue;
						break;
					}
				}
			}
		}
		fclose(file);
	}
	return false;
}
