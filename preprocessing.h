
#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <gst/gst.h>

/**
 * @brief Create a preprocessing bin
 *
 * This function creates a GStreamer bin containing the 'insertbin' element for preprocessing.
 *
 * @return GstElement* The preprocessing bin element
 */
GstElement *create_preprocessing(void);


/**
 * @brief Adds a cameracalibrate element to the insertbin inside the preprocessing bin.
 *
 * @return TRUE if successful, FALSE otherwise.
 */
gboolean calibrate(void);

#endif // PREPROCESSING_H
