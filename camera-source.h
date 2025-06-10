#ifndef CAMERA_SOURCE_H
#define CAMERA_SOURCE_H

#include <gst/gst.h>


/**
 * @brief Create a camera source
 *
 * Camera is connected to Raspberry Pi via CSI interface.
 * 
 * 
 * @param [param_name] [Description of the parameter]
 * @return [Description of the return value]
 * @note [Any additional notes]
 * @warning [Any warnings about usage]
 * @see [References to related functions or documentation]
 */
GstElement *create_camera_source(void);

#endif // CAMERA_SOURCE_H