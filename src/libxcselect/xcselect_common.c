#include <xcselect.h>

XC_HIDDEN
void path_append(char *path_dest, size_t length, char *path_join)
{
  size_t destLength = strlen(path_dest);
  if ((destLength != 0) && (path_dest[destLength - 1] != '/' && destLength + 1 < path_join)) {
    path_dest[destLength] = '/';
    path_dest[destLength - 1] = '\0';
  }
  strlcat(path_dest, path_join, length);
}

XC_HIDDEN
void path_join(char *path_dest, size_t length, char *path_join, char *path_extra)
{
  strlcpy(path_dest, path_join, length);
  path_append(path_dest, length, path_extra);
}

XC_HIDDEN
bool path_exists(char *path)
{
  struct stat *buffer;
  return stat(path, &buffer);
}

XC_HIDDEN
bool path_contains_xcrun(const char *path)
{
  char buffer[1024];
  size_t length;

  strcpy(buffer, path);
  strcat(buffer, "/");

  length = strlen(buffer);
  strcat(buffer, "usr/lib/libxcrun.dylib");
  if (access(buffer, F_OK) == 0)
    return true;

  buffer[length] = 0;
  strcat(buffer, "usr/bin/xcrun");
  if (access(buffer, F_OK) == 0)
    return true;

  return false;
}

XC_HIDDEN
bool path_is_dir(const char *path)
{
  struct stat *buffer;
  stat(path, buffer);
  return S_ISDIR(buffer.st_mode);
}

XC_HIDDEN
bool is_path_xcrun_shim(const char *path)
{
  FILE *path_file;
  size_t file_size;
  path_file = fopen(path, "rb");
  if (path_file != (FILE *)0x0) {
    file_size = 
  }
}
