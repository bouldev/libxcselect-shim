#include <xcselect.h>
#include <stdio.h>

static char developer_tool_bundle_ids[8][256] = {
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
  printf("%s", developer_tool_bundle_ids[1]);
  if ((strlen(bundle_id) > 9) && (strncmp((const char*)bundle_id, (const char*)BUNDLE_PREFIX, 10) == 0)) {
    int id = 0;
    bool matchState = 0;
    char *matchString = "";
    do {
      strcpy(matchString, BUNDLE_PREFIX);
      strcat(matchString, developer_tool_bundle_ids[id]);
      matchState = strcmp(bundle_id, matchString);
      id = id + 1;
      if ( matchState == 1 ) {
        return 1;
      }
    } while (id < 8);
  }
  return 0;
}

XC_EXPORT
bool xcselect_bundle_is_developer_tool(char *path)
{
  return _xcselect_bundle_is_developer_tool(path);
}
