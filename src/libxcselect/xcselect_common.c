#include <xcselect.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

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
	return stat(path, &st)==0;
}

XC_HIDDEN
bool path_contains_xcrun(const char *path)
{
	char buffer[1024];
	struct stat st;
	path_join(buffer, 1024, path, "usr/lib/libxcrun.dylib");
	if(!stat(buffer,&st)&&(st.st_mode&0xF000)==0x8000) {
		return true;
	}
	path_join(buffer, 1024, path, "usr/bin/xcrun");
	if(stat(buffer,&st))return false;
	if((st.st_mode&0xF000)==0x8000) {
		return (st.st_mode>>6)&1;
	}
	return false;
	/*size_t length;

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

	return false;*/
}

XC_HIDDEN
bool path_is_dir(const char *path)
{
	struct stat st;
	bool r=stat(path, &st);
	if(!r)return r;
	return S_ISDIR(st.st_mode);
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
