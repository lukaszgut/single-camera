#include <gst/gst.h>
#include <gst/gstbin.h>
#include <gst/insertbin/gstinsertbin.h>
#include "preprocessing.h"

// Static pointer to insertbin, only accessible in this file
static GstElement *insertbin = NULL;

/**
 * @brief Adds a cameracalibrate element to the insertbin inside the preprocessing bin.
 *
 * @return TRUE if successful, FALSE otherwise.
 */
gboolean calibrate(void)
{
    if (!insertbin) {
        g_printerr("insertbin is not initialized\n");
        return FALSE;
    }

    GstElement *cameracalibrate = gst_element_factory_make("cameracalibrate", "cameracalibrate");
    if (!cameracalibrate) {
        g_printerr("Failed to create cameracalibrate element\n");
        return FALSE;
    }

    // Use gst_insert_bin_append with 4 arguments: insertbin, cameracalibrate, callback, user_data
    gst_insert_bin_append(GST_INSERT_BIN(insertbin), cameracalibrate, NULL, NULL);
    return TRUE;
}

#include "preprocessing.h"

GstElement *create_preprocessing(void)
{
    GstElement *bin = gst_bin_new("preprocessing_bin");
    if (!bin) {
        g_printerr("Failed to create preprocessing bin\n");
        return NULL;
    }

    insertbin = gst_element_factory_make("insertbin", "insertbin");
    if (!insertbin) {
        g_printerr("Failed to create insertbin element\n");
        gst_object_unref(bin);
        return NULL;
    }

    gst_bin_add(GST_BIN(bin), insertbin);

    GstPad *sink_pad = gst_element_get_static_pad(insertbin, "sink");
    GstPad *ghost_sink = gst_ghost_pad_new("sink", sink_pad);
    gst_object_unref(sink_pad);
    gst_element_add_pad(bin, ghost_sink);

    GstPad *src_pad = gst_element_get_static_pad(insertbin, "src");
    GstPad *ghost_src = gst_ghost_pad_new("src", src_pad);
    gst_object_unref(src_pad);
    gst_element_add_pad(bin, ghost_src);

    return bin;
}
