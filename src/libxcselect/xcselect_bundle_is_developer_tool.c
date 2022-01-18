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
bool xcselect_bundle_is_developer_tool(char *path)
{
	return _xcselect_bundle_is_developer_tool(path);
}
