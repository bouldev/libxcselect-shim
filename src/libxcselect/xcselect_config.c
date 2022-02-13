/*
 *  This was not part of original libxcselect.
 *  We are doing this for controlling its behavior,
 *  since most things are not capable for non-macOS.
 */
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <xcselect.h>

static struct {
	char ***config_entries;
	bool config_available;
} xcselect_config = {NULL, true};

XC_HIDDEN
void xcselect_free_config()
{
	xcselect_config.config_available = true;
	if (xcselect_config.config_entries) {
		for (char ***item = xcselect_config.config_entries; *item != NULL; item++) {
			char **p = *item;
			free(p[0]);
			free(p[1]);
			free(p);
		}
		free(xcselect_config.config_entries);
		xcselect_config.config_entries = NULL;
	}
}

XC_HIDDEN
char *xcselect_get_config_string(const char *key)
{
	if (xcselect_config.config_available && !xcselect_config.config_entries) {
		xcselect_config.config_available = true;
		if (xcselect_config.config_available) {
			FILE *config_file = fopen(_PATH_XCSELECT_CONF, "rb");
			if (!config_file) {
				xcselect_config.config_available = false;
				return NULL;
			}
			char *item_name = NULL;
			xcselect_config.config_entries = calloc(32, sizeof(void *));
			int configArrSize = 32;
			int configItemIndex = 0;
			char parseBuf[1024] = {0};
			int itemIndex = 0;
			unsigned short parseIndex = 0;
			int c;
			int lineNumber = 1;
			int isComment = 0;
			while (1)
			{
				c = fgetc(config_file);
				if (c == EOF) {
					if (itemIndex == 1) {
						c = '\n';
					} else if (parseIndex == 0) {
						fclose(config_file);
						if (configItemIndex == configArrSize) {
							xcselect_config.config_entries = realloc(xcselect_config.config_entries, sizeof(void *) * (configArrSize++));
						}
						xcselect_config.config_entries[configItemIndex] = NULL;
						goto finalize;
					} else {
						fprintf(stderr, "Syntax error: config file " _PATH_XCSELECT_CONF ": early EOF at line %d\n", lineNumber);
						fclose(config_file);
						xcselect_config.config_available = false;
						for (int i = 0; i < configItemIndex; i++) {
							free(xcselect_config.config_entries[i][0]);
							free(xcselect_config.config_entries[i][1]);
							free(xcselect_config.config_entries[i]);
						}
						if (item_name != NULL) {
							free(item_name);
						}
						return NULL;
					}
				}

				if (c == '\t' || c == ' ' || c == '\r') continue;

				if (c == '#') {
					isComment++;
					continue;
				}

				if (!isComment) {
					if (itemIndex == 0 && c == '=') {
						parseBuf[parseIndex] = '\0';
						int parseBufLen = parseIndex + 1;
						item_name = malloc(parseBufLen);
						memcpy(item_name, parseBuf, parseBufLen);
						parseIndex = 0;
						itemIndex++;
						continue;
					}
				}
				if (c == '\n') {
					isComment = false;
					if (itemIndex == 0) {
						if (parseIndex == 0)
							continue;
						fprintf(stderr, "Syntax error: config file " _PATH_XCSELECT_CONF ": Invalid line %d, no value assigned!\n", lineNumber);
						fclose(config_file);
						xcselect_config.config_available = false;
						free(item_name);
						for (int i = 0; i < configItemIndex; i++) {
							free(xcselect_config.config_entries[i][0]);
							free(xcselect_config.config_entries[i][1]);
							free(xcselect_config.config_entries[i]);
						}
						return NULL;
					}
					parseBuf[parseIndex] = '\0';
					itemIndex = 0;
					char *item_value = malloc(parseBufLen);
					int parseBufLen = parseIndex + 1;
					memcpy(item_value, parseBuf, parseBufLen);
					parseIndex = 0;
					lineNumber++;
					if (configItemIndex == configArrSize) {
						xcselect_config.config_entries = realloc(xcselect_config.config_entries, (configArrSize + 32) * sizeof(void *));
						configArrSize += 32;
					}
					char **current_item = calloc(2, sizeof(char *));
					current_item[0] = item_name;
					current_item[1] = item_value;
					xcselect_config.config_entries[configItemIndex] = current_item;
					item_name = NULL;
					configItemIndex++;
					continue;
				}
				if (isComment) continue;

				if (parseIndex == 1024) {
					fprintf(stderr, "Syntax error: Property name or value too long at line %d of config file " _PATH_XCSELECT_CONF "\n", lineNumber);
					fclose(config_file);
					xcselect_config.config_available = false;
					for (int i = 0; i < configItemIndex; i++) {
						free(xcselect_config.config_entries[i][0]);
						free(xcselect_config.config_entries[i][1]);
						free(xcselect_config.config_entries[i]);
					}
					if (item_name != NULL) {
						free(item_name);
					}
					return NULL;
				}
				parseBuf[parseIndex] = c;
				parseIndex++;
			}
		} else {
			fprintf(stderr, "Error: config file %s not exist, using default logics\n", _PATH_XCSELECT_CONF);
		}
	}
	if (!xcselect_config.config_available) return NULL;

	for (char ***item_group_ptr = xcselect_config.config_entries; *item_group_ptr != NULL; item_group_ptr++) {
		char **item_group = *item_group_ptr;
		if (strcmp(item_group[0], key) == 0) {
			return item_group[1];
		}
	}
	return NULL;
}
