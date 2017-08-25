#ifndef _GST_OPENCV_H_
#define _GST_OPENCV_H_

#include <stdlib.h>

#include <gst/allocators/gstdmabuf.h>
#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <gst/video/gstvideometa.h>
#include <gst/video/navigation.h>
#include <gst/video/videooverlay.h>

#include <opencv2/opencv.hpp>

#include "gst/GstAppSinkPipeline.h"
#include "gst/GstAppSrcPipeline.h"
#include "gst/GstreamerPipelines.h"

#include "opencv/OpenCVFaceDect.h"
#include "opencv/OpenCVEffect.h"

#endif