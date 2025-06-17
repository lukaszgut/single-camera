# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(single_camera C)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)

find_library(GST_LIBRARIES
		NAMES gstreamer-1.0 gstreamer-base-1.0 libcamera gst-plugins-bad
    		PATHS /home/lukasz/src/gstreamer/install/lib
	)

message(STATUS "GStreamer libraries: ${GST_LIBRARIES}")

set(GST_INCLUDE_DIRS
		${GLIB_INCLUDE_DIRS}
		/home/lukasz/src/robot/gstreamer/install/include/gstreamer-1.0
		/home/lukasz/src/robot/gstreamer/install/include/gstreamer-1.0/gst
		/home/lukasz/src/robot/gstreamer/install/include/gstreamer-1.0/gst/base)

# Include directories
include_directories(
    ${GST_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Source files
set(SOURCES
    main.c
    camera-source.c
    preprocessing.c
    streaming.c
)

# Add executable
add_executable(single_camera ${SOURCES})

# Link GStreamer libraries
target_link_libraries(single_camera ${GST_LIBRARIES} ${GLIB_LIBRARIES})
#target_compile_options(single_camera PRIVATE ${GST_CFLAGS_OTHER})
