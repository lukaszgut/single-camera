#include <gst/gst.h>
#include <gst/gstbin.h>
#include "preprocessing.h"

// Static variable to control calibration mode
static gboolean calibration_mode = TRUE;

void calibrate(void)
{
    calibration_mode = TRUE;
}

void undistort(void)
{
    calibration_mode = FALSE;
}

GstElement *create_preprocessing(void)
{
    GstElement *bin = gst_bin_new("preprocessing_bin");
    if (!bin) {
        g_printerr("Failed to create preprocessing bin\n");
        return NULL;
    }

    GstElement *vc_before = gst_element_factory_make("videoconvert", "vc_before");
    GstElement *effect = NULL;
    GstElement *vc_after = gst_element_factory_make("videoconvert", "vc_after");

    if (calibration_mode) {
        g_print("Creating cameracalibrate effect\n");
        effect = gst_element_factory_make("cameracalibrate", "cameracalibrate");
    } else {
        g_print("Creating cameraundistort effect\n");
        effect = gst_element_factory_make("cameraundistort", "cameraundistort");
    }

    if (vc_before && effect && vc_after) {
        gst_bin_add_many(GST_BIN(bin), vc_before, effect, vc_after, NULL);
        if (!gst_element_link_many(vc_before, effect, vc_after, NULL)) {
            g_printerr("Failed to link videoconvert/effect chain in preprocessing\n");
        }
    } else {
        g_printerr("Failed to create videoconvert or effect element for preprocessing\n");
    }

    GstPad *sink_pad = gst_element_get_static_pad(vc_before, "sink");
    GstPad *ghost_sink = gst_ghost_pad_new("sink", sink_pad);
    gst_object_unref(sink_pad);
    gst_element_add_pad(bin, ghost_sink);

    GstPad *src_pad = gst_element_get_static_pad(vc_after, "src");
    GstPad *ghost_src = gst_ghost_pad_new("src", src_pad);
    gst_object_unref(src_pad);
    gst_element_add_pad(bin, ghost_src);

    return bin;
}
