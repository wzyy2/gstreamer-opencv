#include "gst_opencv.h"

#include "OpenCVStream.h"

//#define HARD_DEC
//#define RTSP_TEST
// #define DEBUG_FPS

// GstFlowReturn
// gst_opencv_handle_sample(GstAppSink* appsink, gpointer data)
// {
//     GstOpencv *self = (GstOpencv *)data;

//     GstSample* sample = gst_app_sink_pull_sample(appsink);
//     GstCaps* caps = gst_sample_get_caps(sample);
//     GstBuffer* buffer = gst_sample_get_buffer(sample);
//     const GstStructure* info = gst_sample_get_info(sample);

//     GstMapInfo map;
//     gst_buffer_map(buffer, &map, GST_MAP_READ);
//     printf("JACOB %s %d\n", __func__, __LINE__);
//     // convert gstreamer data to OpenCV Mat, you could actually
//     // resolve height / width from caps...
//     /*int height,width;
//    GstStructure *s;
//    s = gst_caps_get_structure(caps, 0);
//    gst_structure_get_int(s, "height", &height);
//    gst_structure_get_int(s, "width", &width);
//    Mat frame(Size(1280, 720), CV_YUV2BGR_I420, (char*)map.data, Mat::AUTO_STEP);
//    int frameSize = map.size;*/

//     cv::Mat frame(1080, 1920, CV_8UC3);
//     cv::Mat src(1080, 1920, CV_8UC1, (char*)map.data);
// #ifdef HARD_DEC
//     cv::cvtColor(src, frame, CV_YUV2BGR_NV12);
// #else
//     cv::cvtColor(src, frame, CV_YUV2BGR_I420);
// #endif

//     gst_buffer_unmap(buffer, &map);

//     // show caps on first frame
//     //g_print("%s\n", gst_caps_to_string(caps));

//     gst_sample_unref(sample);

//     return GST_FLOW_OK;
// }

int main(int argc, char* argv[])
{
    OpenCVStream *stream;

    stream = new OpenCVStream();
    stream->StreamON();

    std::cout << "Enter to leave!" << std::endl;

    getchar();

    stream->StreamOFF();

    return 0;
}
