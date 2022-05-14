# libxcselect-shim

WIP reimplementation of Xcode's libxcselect and related CLI tools.

## Current status

- Compiles on/for macOS and iOS

- An already working config feature

- Functions that passed tests (All done)
    - xcselect_bundle_is_developer_tool
    - xcselect_developer_dir_matches_path
    - xcselect_find_developer_contents_from_path
    - xcselect_get_developer_dir_path
    - xcselect_get_manpaths
    - xcselect_get_version
    - xcselect_host_sdk_path
    - xcselect_invoke_xcrun
    - xcselect_manpaths_free
    - xcselect_manpaths_get_num_paths
    - xcselect_manpaths_get_path
    - xcselect_trigger_install_request

- Configurable functions
    - xcselect_bundle_is_developer_tool
    - xcselect_developer_dir_matches_path

- Modifications
    - xcselect_host_sdk_path now reads SDKROOT
    - (TODO) xcselect_trigger_install_request triggers APT to install requested packages on non-macOS
