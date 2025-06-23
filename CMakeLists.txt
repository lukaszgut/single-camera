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
    camera-source.c
    preprocessing.c
    streaming.c
)

add_executable(single_camera ${SOURCES})

target_compile_definitions(single_camera PRIVATE
    GST_USE_UNSTABLE_API
)

target_link_libraries(single_camera ${GST_LIBRARIES} ${GLIB_LIBRARIES} ${GOB_LIBRARIES})
#target_compile_options(single_camera PRIVATE ${GST_CFLAGS_OTHER})
