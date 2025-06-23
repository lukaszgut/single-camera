#include <gst/gst.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "camera-source.h"
#include "preprocessing.h"
#include "streaming.h"

GstElement *pipeline = NULL;

void run_pipeline(int argc, char *argv[]) {
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

    // Wait until error or EOS
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Free resources
    if (msg)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

void *run_control_loop(void *arg) {
	sleep(10);
	g_print("Calling calibrate\n");
    while (1) {
        sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t control_thread;
    // Spawn the control loop thread
    if (pthread_create(&control_thread, NULL, run_control_loop, NULL) != 0) {
        g_printerr("Failed to create control loop thread\n");
        return -1;
    }
    run_pipeline(argc, argv);
    pthread_join(control_thread, NULL);
    return 0;
}