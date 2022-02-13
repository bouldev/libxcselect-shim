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

static struct {
	char ***config_entries=NULL;
	bool config_available=true;
} xcselect_config;

static void
xcselect_free_config() {
	xcselect_config.config_available=true;
	if(xcselect_config.config_entries) {
		for(char ***item=xcselect_config.config_entries;item!=NULL;item++) {
			char **p=*item;
			free(p[0]);
			free(p[1]);
			free(p);
		}
		free(xcselect_config.config_entries);
		xcselect_config.config_entries=NULL;
	}
}

static char *
xcselect_get_config_string(const char *key)
{
	if (xcselect_config.config_available&&!xcselect_config.config_entries) {
		xcselect_config.config_available = path_exists(_PATH_XCSELECT_CONF);
		if (xcselect_config.config_available) {
			FILE *config_file=fopen(_PATH_XCSELECT_CONF, "rb");
			if(!config_file) {
				xcselect_config.config_available=false;
				return;
			}
			char *item_name=NULL;
			config_entries=calloc(32,sizeof(void *));
			int configArrSize=32;
			int configItemIndex=0;
			char parseBuf[1024]={0};
			int itemIndex=0;
			unsigned short parseIndex=0;
			int c;
			int lineNumber=1;
			int isComment=0;
			while(1) {
				c=fgetc(config_file);
				if(c==EOF) {
					if(itemIndex==1) {
						c='\n';
					}else if(parseIndex==0) {
						fclose(config_file);
						if(configItemIndex==configArrSize) {
							config_entries=reallocarray(config_entries, configArrSize++, sizeof(void*));
						}
						config_entries[configItemIndex]=NULL;
						return;
					}else{
						fprintf(stderr, "Syntax error: config file " _PATH_XCSELECT_CONF ": early EOF at line %d.",lineNumber);
						fclose(config_file);
						xcselect_config.config_available=false;
						for(int i=0;i<configItemIndex;i++) {
							free(config_entries[i][0]);
							free(config_entries[i][1]);
							free(config_entries[i]);
						}
						if(item_name!=NULL) {
							free(item_name);
						}
						return;
					}
				}
				if(c=='\t'||c==' '||c=='\r') {
					continue;
				}
				if(c=='#') {
					isComment++;
					continue;
				}
				if(!isComment) {
					if(itemIndex==0&&c=='=') {
						int parseBufLen=strlen(parseBuf)+1;
						item_name=malloc(parseBufLen);
						memcpy(item_name,parseBuf,parseBufLen);
						parseIndex=0;
						itemIndex++;
						continue;
					}
				}
				if(c=='\n') {
					isComment=false;
					if(itemIndex==0) {
						fprintf(stderr, "Syntax error: config file " _PATH_XCSELECT_CONF ": Invalid line %d, no value assigned!",lineNumber);
						fclose(config_file);
						xcselect_config.config_available=false;
						free(item_name);
						for(int i=0;i<configItemIndex;i++) {
							free(config_entries[i][0]);
							free(config_entries[i][1]);
							free(config_entries[i]);
						}
						return;
					}
					itemIndex=0;
					parseIndex=0;
					lineNumber++;
					int parseBufLen=strlen(parseBuf)+1;
					char *item_value=malloc(parseBufLen);
					memcpy(item_value,parseBuf,parseBufLen);
					if(configItemIndex==configArrSize) {
						config_entries=reallocarray(config_entries, configArrSize+32, sizeof(void*));
					}
					char **current_item=calloc(2,sizeof(char *));
					current_item[0]=item_name;
					current_item[1]=item_value;
					config_entries[configItemIndex]=current_item;
					item_name=NULL;
					configItemIndex++;
					continue;
				}
				if(isComment)
					continue;
				if(parseIndex==1024) {
					fprintf(stderr, "Syntax error: Property name or value too long at line %d of config file " _PATH_XCSELECT_CONF ".", lineNumber);
					fclose(config_file);
					xcselect_config.config_available=false;
					for(int i=0;i<configItemIndex;i++) {
						free(config_entries[i][0]);
						free(config_entries[i][1]);
						free(config_entries[i]);
					}
					if(item_name!=NULL) {
						free(item_name);
					}
					return;
				}
				parseBuf[parseIndex]=c;
			}
		} else {
			// path_exists set errno to ENOENT if path not exist
			fprintf(stderr, "Error: config file %s not exist, using default logics (%s)\n", _PATH_XCSELECT_CONF, strerror(errno));
		}
	}
	if(!xcselect_config.config_available) {
		return NULL;
	}
	for(char ***item_group_ptr=xcselect_config.config_entries;item_group_ptr!=NULL;item_group_ptr++) {
		char **item_group=*item_group_ptr;
		if(strcmp(item_group[0], key)==0) {
			return item_group[1];
		}
	}
	return NULL;
}
