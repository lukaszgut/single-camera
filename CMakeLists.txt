# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(single_camera C)

# Find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.14 gstreamer-base-1.0 libcamera)
message(STATUS "GStreamer libraries: ${GST_LIBRARIES}")

# Include directories
include_directories(
    ${GST_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Source files
set(SOURCES
    main.c
    camera-source.c
    streaming.c
)

# Add executable
add_executable(single_camera ${SOURCES})

# Link GStreamer libraries
target_link_libraries(single_camera ${GST_LIBRARIES})
target_compile_options(single_camera PRIVATE ${GST_CFLAGS_OTHER})
