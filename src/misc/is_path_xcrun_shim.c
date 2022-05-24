#include <xcselect.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <dispatch/dispatch.h>
#include <sys/_endian.h>

XC_HIDDEN
bool is_path_xcrun_shim(const char *path)
{

	FILE *file, *file_op;
	uint32_t fat_cigam_buf;
	int op_8[3];
	struct mach_header_64 mach_header;
	struct section_64 section;
	int32_t section_swapped;
	long offset;
	int ret;

	file = fopen(path, "rb");
	if (!file) return false;

	file_op = file;
	if (fread(&fat_cigam_buf, 4, 1, file) == 1) {
		if (fat_cigam_buf != FAT_CIGAM) { // IDA: ntohl(fat_cigam_buf) but not really
			offset = 0;
			goto find_mh_magic;
		}

		if (fseek(file_op, 0, SEEK_SET) != -1
		 && fread(op_8, 8, 1, file_op) == 1
		 && fread(&section, 20, 1, file_op) == 1) {
			offset = htonl(*(uint32_t *)&section.sectname[8]);
find_mh_magic:
			if (fseek(file_op, offset, SEEK_SET) != -1
			 && fread(&mach_header, 32, 1, file_op) == 1
			 && mach_header.magic == MH_MAGIC_64) {
				int ncmds = mach_header.ncmds;
				int ncmds_swap;
				long int current_pos;
				struct segment_command_64 segment_command;
				if (ncmds) {
					ncmds_swap = 0;
					while (true) {
						current_pos = ftell(file_op);
						if (fread(op_8, 8, 1, file_op) != 1)
							break;

						if (op_8[0] == LC_SEGMENT_64) {
							if (fseek(file_op, current_pos, 0) == -1
							 || fread(&segment_command, 72, 1, file_op) != 1) break;

							if (!(*(uint32_t *)segment_command.segname ^ 'AD__'
							 | *(uint32_t *)&segment_command.segname[3] ^ '\0ATA')) { // "__DATA"
								uint32_t xcrun_shim = segment_command.nsects;
								if (segment_command.nsects) {
									while (fread(&section, 80, 1, file_op) == 1) {
										if (!(*(uint64_t *)section.sectname ^ 0x5F6E757263785F5F
										 | *(uint64_t *)&section.sectname[5] ^ 0x6D6968735F6E75)) { // "__xcrun_shim"
											ret = 1;
											goto close_and_exit;
										}
										if (!--xcrun_shim) goto read_and_break;
									}
									break;
								}
							}
						}
read_and_break:
						if (fseek(file_op, current_pos + op_8[1], SEEK_SET) == -1) break;
						ret = false;
						if (++ncmds_swap == ncmds) goto close_and_exit;
					}
				}
			}
		}
	}
	ret = 0;
close_and_exit:
	fclose(file_op);
	return ret;
}
