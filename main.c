#include <gst/gst.h>
#include <stdio.h>

#include "camera-source.h"
#include "preprocessing.h"
#include "streaming.h"

#define GST_USE_UNSTABLE_API

int main(int argc, char *argv[]) {
	GstElement *pipeline;
	GError *error = NULL;

	// Initialize GStreamer
	gst_init(&argc, &argv);

	GstElement *camera_source = create_camera_source();
	GstElement *preprocessing = create_preprocessing();
	GstElement *streaming = create_streaming();

	if (!camera_source || !preprocessing || !streaming) {
		g_printerr("Failed to create elements\n");
		return -1;
	}
	pipeline = gst_pipeline_new("main-pipeline");
	if (!pipeline) {
		g_printerr("Failed to create pipeline: %s\n", error ? error->message : "unknown error");
		if (error) g_clear_error(&error);
		return -1;
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
		return -1;
	}

	calibrate(); // Call the calibrate function to add cameracalibrate to the insertbin

	// Start playing
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

	return 0;
}