#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <gst/gst.h>

/**
 * @brief Create a preprocessing bin
 *
 * This function creates a GStreamer bin containing the calibration or undistort effect for preprocessing.
 *
 * @return GstElement* The preprocessing bin element
 */
GstElement *create_preprocessing(void);

/**
 * @brief Set preprocessing to calibration mode (use cameracalibrate effect)
 */
void calibrate(void);

/**
 * @brief Set preprocessing to undistort mode (use cameraundistort effect)
 */
void undistort(void);


#endif // PREPROCESSING_H
