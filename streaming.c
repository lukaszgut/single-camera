#include "streaming.h"

GstElement *create_streaming(void)
{
    	GstElement *bin = gst_bin_new("streaming_bin");
	if (!bin) {
		g_printerr("Failed to create streaming bin\n");
		return NULL;
	}
	GstElement *sink = gst_element_factory_make("tcpserversink", "tcp_sink");
	if (!sink) {
		gst_object_unref(bin);
		g_printerr("Failed to create UDP sink\n");
		return NULL;
	}

	// Set UDP sink properties (example: host=127.0.0.1, port=5000)
	g_object_set(sink, "host", "0.0.0.0", "port", 5000, NULL);

	gst_bin_add(GST_BIN(bin), sink);

	GstPad *pad = gst_element_get_static_pad(sink, "sink");
	GstPad *ghost_pad = gst_ghost_pad_new("sink", pad);
	gst_object_unref(pad);

	gst_element_add_pad(bin, ghost_pad);

	return bin;
}
