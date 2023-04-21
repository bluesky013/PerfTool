cmake_minimum_required(VERSION 3.10.0)

project(PerfTool)

file(GLOB_RECURSE SRC_FILES src/*)
file(GLOB_RECURSE INC_FILES include/*)

if (WIN32)
    file(GLOB_RECURSE PLATFORM_FILES platform/Windows/*)
else()
endif()

add_executable(PerfTool
        ${SRC_FILES}
        ${INC_FILES}
        ${PLATFORM_FILES}
)

target_include_directories(PerfTool PUBLIC
        include
        src)
