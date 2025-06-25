#include <gst/gst.h>
#include <gst/gstbin.h>
#include "preprocessing.h"
#include <stdio.h>
#include <stdlib.h>

// Static variable to control calibration mode
static gboolean calibration_mode = TRUE;
// Static pointer to the effect element
static GstElement *effect = NULL;
static const char *CALIBRATION_ENV_VAR = "ROBOT_CAMERA_STREAM_CALIBRATION_PATH";

char *read_calibration_data(void);

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
    effect = NULL;
    GstElement *vc_after = gst_element_factory_make("videoconvert", "vc_after");

    if (calibration_mode) {
        g_print("Creating cameracalibrate effect\n");
        effect = gst_element_factory_make("cameracalibrate", "cameracalibrate");
        if (effect) {
            // Set board property to 6x6
            g_object_set(effect, "board-width", 6, "board-height", 6, NULL);
        }
    } else {
        g_print("Creating cameraundistort effect\n");
        effect = gst_element_factory_make("cameraundistort", "cameraundistort");
	char *calibration_data = read_calibration_data();
	if (calibration_data) {
	    g_print("Using existing calibration data: %s\n", calibration_data);
	    g_object_set(effect, "settings", calibration_data, NULL);
	    free(calibration_data);
	} else {
	    g_printerr("No calibration data found, using default settings\n");
	}
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

const char *get_calibration_path(void) {
    const char *env_path = getenv(CALIBRATION_ENV_VAR);
    if (env_path && env_path[0] != '\0') {
        return env_path;
    }
    return "calibration_data.xml";
}

void save_calibration_data(const gchar *settings) {
    if (!settings) return;
    const char *path = get_calibration_path();
    FILE *f = fopen(path, "w");
    if (!f) {
        g_printerr("Failed to open %s for writing\n", path);
        return;
    }
    fprintf(f, "%s\n", settings);
    fclose(f);
    g_print("Calibration data saved to %s\n", path);
}

char *read_calibration_data(void) {
    const char *path = get_calibration_path();
    FILE *f = fopen(path, "r");
    if (!f) {
        g_printerr("Failed to open %s for reading\n", path);
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    long size = ftell(f);
    if (size < 0) {
        fclose(f);
        return NULL;
    }
    rewind(f);
    char *buffer = (char *)malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }
    size_t read_size = fread(buffer, 1, size, f);
    fclose(f);
    if (read_size != (size_t)size) {
        free(buffer);
        return NULL;
    }
    buffer[size] = '\0';
    return buffer;
}

void handle_calibration_message(GstElement *pipeline) {
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
                save_calibration_data(settings);
            }
        }
    }
    if (msg) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
}
