#include <xcselect.h>

static char *developer_tool_bundle_ids[] = {
	"dt.Xcode",
	"dt.SourceEdit",
	"iphonesimulator",
	"dt.Instruments",
	"FileMerge",
	"itunes.connect.ApplicationLoader",
	"AccessibilityInspector",
	"RealityComposer"
};

static bool _xcselect_bundle_is_developer_tool(const char *bundle_id)
{
	int size = 0;
	int n = 1;
	char *buf = NULL;
	char **bundles;
	char *config = xcselect_get_config_string("developer_tool_bundle_ids");
	if (config) {
		for (char *sep = config; *sep; sep++) {
			if (*sep == *BUNDLE_SEPARATOR) n++;
		}
		bundles = malloc(sizeof(void *) * n);

		char *p = strtok_r(config, BUNDLE_SEPARATOR, &buf);
		while (p != NULL) {
			bundles[size] = p;
			size++;
			p = strtok_r(NULL, BUNDLE_SEPARATOR, &buf);
		}

		for (int i = 0; i < size; ++i) {
			if (strcmp(bundles[i], bundle_id) == 0) return true;
		}

		free(bundles);
	}
	//printf("%s", developer_tool_bundle_ids[1]);
	if ((strlen(bundle_id) >= 10) && strncmp((const char*)bundle_id, (const char*)BUNDLE_PREFIX, 10) == 0) {
		char matchString[128];
		sprintf(matchString, BUNDLE_PREFIX);
		char *matchChild = matchString + 10;
		for(int i = 0; i < 8; i++) {
			strcpy(matchChild, developer_tool_bundle_ids[i]);
			if(strcmp(matchString,bundle_id) == 0) {
				return true;
			}
		}
	}
	return false;
}

XC_EXPORT
bool xcselect_bundle_is_developer_tool(char *bundle_id)
{
	if (xcselect_config_get_bool(xcselect_get_config_string("allow_any_bundles"))) return true;
	return _xcselect_bundle_is_developer_tool(bundle_id);
}
