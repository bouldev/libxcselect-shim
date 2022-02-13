#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dispatch/dispatch.h>
#include <xcselect.h>

static char *strtolower(char *s)
{
    char *d = (char *)malloc(strlen(s));
    while (*s)
    {
        *d =tolower(*s);
        d++;
        s++;
    }
    return d;
}

static bool
xcselect_get_config(const char *key, char *value)
{
	static struct {
		dispatch_once_t onceToken;
		bool config_exists;
		bool config_readable;
		FILE *config_open;
	} xcselect_config;

	if (xcselect_config.onceToken != -1) {
		dispatch_once(&xcselect_config.onceToken, ^{
			xcselect_config.config_exists = path_exists(_PATH_XCSELECT_CONF);
			if (xcselect_config.config_exists) {
				if (xcselect_config.config_open = fopen(_PATH_XCSELECT_CONF, "r"), xcselect_config.config_open != NULL) {
					xcselect_config.config_readable = true;
				} else {
					xcselect_config.config_readable = false;
					fprintf(stderr, "Syntax error: config file %s not readable (%s)", _PATH_XCSELECT_CONF, strerror(errno));
				}
			} else {
				// path_exists set errno to ENOENT if path not exist
				fprintf(stderr, "Syntax error: config file %s not exist, using default logics (%s)\n", _PATH_XCSELECT_CONF, strerror(errno));
			}
		});
	}
	char line[MAXPATHLEN], bool_format[128];
	bool key_is_bool, found_config_key = false;
	char ignore_config_str[] = "Ignored config at line", syntax_err_str[] = "with syntax error:";
	int linenum = 0;

	if (xcselect_config.config_readable) {
		while(fgets(line, MAXPATHLEN, xcselect_config.config_open) != NULL)
		{
			char config_key[256], config_value[MAXPATHLEN];

			linenum++;
			if(line[0] == '#') continue;

			if(sscanf(line, "%s %s", config_key, config_value) != 2) {
				fprintf(stderr, "%s %d %s %s", ignore_config_str, linenum, syntax_err_str, "more than one arguments\n");
				continue;
			}

			if (strcmp(config_key, key) == 0) {
				// Pass config_value to bool_format for us to format input later
				if (strcmp(key, "is_path_xcrun_shim")) {
					key_is_bool = true;
					strcat(bool_format, strtolower(config_value));
				}

				// Multiple config_value for one key
				if (strcmp(key, "developer_tool_bundle_id")) {
					strcpy(config_value, value);
					strcat(config_value, BUNDLE_SEPERATOR);
				}

				// Format bool to string "true" or "false" if have any
				if (key_is_bool) {
					if (strcmp(bool_format, "0")) { strcpy(config_value, "false"); }
					else if (strcmp(bool_format, "1")) { strcpy(config_value, "true"); }
					else if (!strcmp(bool_format, "true") && !strcmp(bool_format, "false")) {
						fprintf(stderr, "%s %d %s %s", ignore_config_str, linenum, syntax_err_str, "not a bool value\n");
						continue;
					}
				}

				value = config_value;
				found_config_key = true;
			}
		}
		fclose(xcselect_config.config_open);
	}
	return (xcselect_config.config_readable) ? found_config_key : false;
}
