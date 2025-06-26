cmake_minimum_required(VERSION 3.10)

project(single_camera C)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(GOB REQUIRED gobject-2.0)

find_library(GST_LIBRARIES
		NAMES gstreamer-1.0 gstreamer-base-1.0 libcamera gst-plugins-bad
    		PATHS /home/lukasz/src/gstreamer/install/lib
	)

message(INFO "GLIB libraries: ${GLIB_LIBRARIES}")

set(GST_INCLUDE_DIRS
		${GLIB_INCLUDE_DIRS}
		/home/lukasz/src/robot/gstreamer/install/include/gstreamer-1.0
		/home/lukasz/src/robot/gstreamer/install/include/gstreamer-1.0/gst
		/home/lukasz/src/robot/gstreamer/install/include/gstreamer-1.0/gst/base)

include_directories(
    ${GST_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SOURCES
    main.c
)

add_executable(stereo-vision-pipeline ${SOURCES})

target_compile_definitions(stereo-vision-pipeline PRIVATE
    GST_USE_UNSTABLE_API
)

target_link_libraries(stereo-vision-pipeline ${GST_LIBRARIES} ${GLIB_LIBRARIES} ${GOB_LIBRARIES})

