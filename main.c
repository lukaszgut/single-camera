#include <gst/gst.h>
#include <stdio.h>

#include "camera-source.h"
#include "preprocessing.h"
#include "dewarping.h"
#include "streaming.h"

int main(int argc, char *argv[]) {
	GstElement *pipeline;
	GError *error = NULL;

	// Initialize GStreamer
	gst_init(&argc, &argv);

	// Example pipeline: videotestsrc ! autovideosink


	GstElement *camera_source = create_camera_source();
	GstElement *streaming = create_streaming();

	if (!camera_source || !streaming) {
		g_printerr("Failed to create elements\n");
		return -1;
	}
	pipeline = gst_pipeline_new("main-pipeline");
	if (!pipeline) {
		g_printerr("Failed to create pipeline: %s\n", error->message);
		g_clear_error(&error);
		return -1;
	}

	gst_bin_add_many(GST_BIN(pipeline),
					 camera_source,
					 streaming,
					 NULL);

	// Link the bins (empty bins, so linking will fail unless pads are added later)
	gst_element_link_many(camera_source, streaming, NULL);



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