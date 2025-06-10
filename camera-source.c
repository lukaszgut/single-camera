#include "camera-source.h"

GstElement * create_camera_source(void)
{
	GstElement *bin = gst_bin_new("camera_source_bin");
	if (!bin) {
	g_printerr("Failed to create camera source bin\n");
	return NULL;
}
	GError *error = NULL;
	GstElement *src = gst_element_factory_make("v4l2src", "camera_src");


	if (!src) {
		g_printerr("Failed to create camera source element: %s\n", error ? error->message : "unknown error");
		if (error) g_clear_error(&error);
		gst_object_unref(bin);
		return NULL;
	}

	gst_bin_add(GST_BIN(bin), src);

	GstPad *pad = gst_element_get_static_pad(src, "src");
	GstPad *ghost_pad = gst_ghost_pad_new("src", pad);
	gst_object_unref(pad);

	gst_element_add_pad(bin, ghost_pad);

	return bin;
}