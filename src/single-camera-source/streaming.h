#ifndef STREAMING_H
#define STREAMING_H

#include <gst/gst.h>

/**
 * @brief Create a streaming element/bin
 *
 * This function creates a GStreamer bin responsible for streaming the video.
 *
 * @return GstElement* The streaming bin element
 */
GstElement *create_streaming(void);

#endif // STREAMING_H
