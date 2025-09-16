
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(ZeekPluginDPDK)

include(ZeekPlugin)

zeek_plugin_begin(ESnet DPDK)
zeek_plugin_cc(src/Plugin.cc src/DPDK.cc)
zeek_plugin_dist_files(README CHANGES COPYING VERSION)
zeek_plugin_link_library(rte_eal rte_ethdev rte_mbuf)
zeek_plugin_end()

option(OPTIMIZE_FOR_NATIVE "Build with -march=native" true)

if(OPTIMIZE_FOR_NATIVE)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
    if(COMPILER_SUPPORTS_MARCH_NATIVE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=x86-64 -msse4.1 -msse3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -msse4.1 -msse3")
endif()

include_directories(/opt/zeek/include)

# Use pkg-config to get DPDK configuration
find_package(PkgConfig REQUIRED)
pkg_check_modules(DPDK REQUIRED libdpdk)

# Add DPDK include directories and compile flags
include_directories(${DPDK_INCLUDE_DIRS})
add_compile_options(${DPDK_CFLAGS_OTHER})

# Add DPDK preprocessor definitions
add_definitions(-DRTE_CONFIG_H_INCLUDED)
foreach(flag ${DPDK_CFLAGS_OTHER})
    if(flag MATCHES "^-D")
        add_definitions(${flag})
    endif()
endforeach()

# Link DPDK libraries
link_directories(${DPDK_LIBRARY_DIRS})
set(DPDK_LIBRARIES ${DPDK_LIBRARIES})

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION LIMIT_COUNT 1)

if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    # Allows building rpm/deb packages via "make package" in build dir.
    include(ConfigurePackaging)
    ConfigurePackaging(${VERSION})
endif ()
