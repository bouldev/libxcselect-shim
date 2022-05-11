# libxcselect-shim

WIP reimplementation of Xcode's libxcselect and related CLI tools.

## Current status

- Compiles

- An already working config feature

- Functions that passed tests
    - xcselect_bundle_is_developer_tool
    - xcselect_developer_dir_matches_path
    - xcselect_find_developer_contents_from_path
    - xcselect_get_developer_dir_path
    - xcselect_get_manpaths
    - xcselect_get_version
    - xcselect_host_sdk_path
    - xcselect_manpaths_free
    - xcselect_manpaths_get_num_paths
    - xcselect_manpaths_get_path

- Configurable functions
    - xcselect_bundle_is_developer_tool
    - xcselect_developer_dir_matches_path
