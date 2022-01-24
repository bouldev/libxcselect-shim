add_library(xcselect
    xcselect_bundle_is_developer_tool.c
    xcselect_common.c
    xcselect_developer_dir_matches_path.c
    xcselect_find_developer_contents_from_path.c
    xcselect_get_developer_dir_path.c
    xcselect_get_manpaths.c
    xcselect_get_version.c
    xcselect_host_sdk_path.c
    xcselect_invoke_xcrun.c
    xcselect_manpaths_free.c
    xcselect_manpaths_get_num_paths.c
    xcselect_manpaths_get_path.c
    xcselect_trigger_install_request.c
)

set_target_properties(xcselect PROPERTIES PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/xcselect.h)
set_target_properties(xcselect PROPERTIES PRIVATE_HEADER ${CMAKE_SOURCE_DIR}/xcselect_private.h)

include(GNUInstallDirs)
target_include_directories(xcselect PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_definitions(xcselect PRIVATE _XC_INTN)

install(TARGETS xcselect
    PUBLIC_HEADER
        COMPONENT Development
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PRIVATE_HEADER
        COMPONENT Development
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    LIBRARY
        COMPONENT Libraries
        NAMELINK_COMPONENT Development
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)