#include "streaming.h"

GstElement *create_streaming(void)
{
    	GstElement *bin = gst_bin_new("streaming_bin");
	if (!bin) {
		g_printerr("Failed to create streaming bin\n");
		return NULL;
	}
	GstElement *jpegenc = gst_element_factory_make("jpegenc", "jpegenc");
	GstElement *avimux = gst_element_factory_make("avimux", "avimux");
	GstElement *sink = gst_element_factory_make("tcpserversink", "tcp_sink");
	if (!jpegenc || !avimux || !sink) {
		g_printerr("Failed to create jpegenc, avimux, or sink\n");
		if (jpegenc) gst_object_unref(jpegenc);
		if (avimux) gst_object_unref(avimux);
		if (sink) gst_object_unref(sink);
		gst_object_unref(bin);
		return NULL;
	}

	// Set sink properties (example: host=0.0.0.0, port=5000)
	g_object_set(sink, "host", "0.0.0.0", "port", 5000, NULL);

	gst_bin_add_many(GST_BIN(bin), jpegenc, avimux, sink, NULL);
	if (!gst_element_link_many(jpegenc, avimux, sink, NULL)) {
		g_printerr("Failed to link jpegenc, avimux, and sink in streaming bin\n");
	}

	GstPad *pad = gst_element_get_static_pad(jpegenc, "sink");
	GstPad *ghost_pad = gst_ghost_pad_new("sink", pad);
	gst_object_unref(pad);
	gst_element_add_pad(bin, ghost_pad);

	return bin;
}
