cmake_minimum_required(VERSION 3.0.0)
project(libxcselect-shim VERSION 0.0.1)

option(BUILD_SHARED_LIBS "build libxcselect-shim libraries as shared lib" ON)
option(ENABLE_COMPILER_WARNINGS "enable compiler warnings" OFF)

if(ENABLE_COMPILER_WARNINGS)
  add_compile_options(
    "-Wall"
    "-Wextra"
    "-Wconversion"
    "-Wsign-conversion"
    "-Wmissing-prototypes"
    "-Werror=implicit-function-declaration"
    "-Werror=incompatible-pointer-types"
    "-Werror=incompatible-sysroot"
  )
endif()

include(CTest)
enable_testing()

add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
