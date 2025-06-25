#include <gst/gst.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "camera-source.h"
#include "preprocessing.h"
#include "streaming.h"

GstElement *pipeline = NULL;

void start_pipeline(int argc, char *argv[]) {
    GError *error = NULL;

    // Initialize GStreamer
    gst_init(&argc, &argv);

    GstElement *camera_source = create_camera_source();
    GstElement *preprocessing = create_preprocessing();
    GstElement *streaming = create_streaming();

    if (!camera_source || !preprocessing || !streaming) {
        g_printerr("Failed to create elements\n");
        return;
    }
    pipeline = gst_pipeline_new("main-pipeline");
    if (!pipeline) {
        g_printerr("Failed to create pipeline: %s\n", error ? error->message : "unknown error");
        if (error) g_clear_error(&error);
        return;
    }

    gst_bin_add_many(GST_BIN(pipeline),
                     camera_source,
                     preprocessing,
                     streaming,
                     NULL);

    // Link the bins: camera_source -> preprocessing -> streaming
    if (!gst_element_link_many(camera_source, preprocessing, streaming, NULL)) {
        g_printerr("Failed to link elements in pipeline\n");
        gst_object_unref(pipeline);
        return;
    }

    // Start playing
    g_print("Starting pipeline...\n");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

}

void close_pipeline() {
    if (pipeline) {
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	pipeline = NULL;
    }
}

void run_calibration(int argc, char *argv[]) {
    calibrate();
    start_pipeline(argc, argv);
    handle_calibration_message(pipeline);
    close_pipeline();
}

void run_streaming(int argc, char *argv[]) {
    undistort();
    start_pipeline(argc, argv);
    handle_calibration_message(pipeline);
    close_pipeline();
}

int main(int argc, char *argv[]) {
    g_print("Calibration starts\n");
    run_calibration(argc, argv);
    g_print("Streaming starts\n");
    run_streaming(argc, argv);
    return 0;
}