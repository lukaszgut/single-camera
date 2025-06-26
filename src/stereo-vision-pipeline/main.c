#include <gst/gst.h>

GstElement* create_tcp_to_window_pipeline(void) {
    GstElement *pipeline = gst_pipeline_new("tcp-to-window-pipeline");
    if (!pipeline) return NULL;

    GstElement *tcpclientsrc = gst_element_factory_make("tcpclientsrc", "tcpclientsrc");
    GstElement *jpegdec = gst_element_factory_make("jpegdec", "jpegdec");
    GstElement *videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
    GstElement *autovideosink = gst_element_factory_make("autovideosink", "autovideosink");

    if (!tcpclientsrc || !jpegdec || !videoconvert || !autovideosink) {
        gst_object_unref(pipeline);
        return NULL;
    }

    g_object_set(tcpclientsrc, "host", "127.0.0.1", "port", 5000, NULL);

    gst_bin_add_many(GST_BIN(pipeline), tcpclientsrc, jpegdec, videoconvert, autovideosink, NULL);

    if (!gst_element_link_many(tcpclientsrc, jpegdec, videoconvert, autovideosink, NULL)) {
        gst_object_unref(pipeline);
        return NULL;
    }

    return pipeline;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    GstElement *pipeline = create_tcp_to_window_pipeline();
    if (!pipeline) {
        g_printerr("Failed to create pipeline\n");
        return -1;
    }
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    if (msg) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}