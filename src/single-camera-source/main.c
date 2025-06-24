#include <gst/gst.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "camera-source.h"
#include "preprocessing.h"
#include "streaming.h"

GstElement *pipeline = NULL;

void handle_calibration_data(const gchar *settings) {
    if (!settings) return;
    FILE *f = fopen("calibration_data.xml", "w");
    if (!f) {
        g_printerr("Failed to open calibration_data.xml for writing\n");
        return;
    }
    fprintf(f, "%s\n", settings);
    fclose(f);
    g_print("Calibration data saved to calibration_data.xml\n");
}

void handle_calibration_message(void) {
    GstBus *bus = gst_element_get_bus(pipeline);
    if (!bus) {
        g_print("Failed to get bus from pipeline\n");
        return;
    }
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ELEMENT);
    g_print("Received message: %s\n", msg ? GST_MESSAGE_TYPE_NAME(msg) : "(null)");
    if (msg && GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ELEMENT) {
        const GstStructure *s = gst_message_get_structure(msg);
        if (s && gst_structure_has_name(s, "CameraCalibration")) {
            const gchar *settings = gst_structure_get_string(s, "serialized-undistort-settings");
            if (settings) {
                g_print("Received calibration settings: %s\n", settings);
                handle_calibration_data(settings);
            }
        }
    }
    if (msg) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
}

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

}

void close_pipeline() {
    if (pipeline) {
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	pipeline = NULL;
    }
}

int main(int argc, char *argv[]) {
    run_pipeline(argc, argv);
    handle_calibration_message();
    close_pipeline();
    return 0;
}