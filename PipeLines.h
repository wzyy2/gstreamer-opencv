#ifndef _PIPELINE_H
#define _PIPELINE_H

#define TEST_VIDEO
//#define TEST_RTSP
//#define TEST_CAMERA

#ifndef TEST_CAMERA
static std::string CreateAppSinkPipeline()
{
    std::stringstream pipelineString;

    pipelineString
#ifdef TEST_RTSP
        << "rtsp://192.168.31.163:554/"
        << LINK
#elif defined TEST_VIDEO
        << "filesrc location=/usr/local/test.mp4"
        << LINK
        << "qtdemux"
        << LINK
#endif
        << "h264parse"
        << LINK
        << "mppvideodec"
        << LINK
        << "video/x-raw,format=(string)NV12"
        << LINK
        << "rgaconvert output-io-mode=dmabuf-import capture-io-mode=dmabuf vpu-stride=true"
        << LINK
        << "video/x-raw,format=BGR,width=(int)1920,height=(int)1080"
        << LINK
        << "appsink caps=video/x-raw,format=BGR name="
        << APPSINK_NAME;

    return pipelineString.str();
}
#else
static std::string CreateAppSinkPipeline()
{
    std::stringstream pipelineString;

    pipelineString
        << "v4l2src device=/dev/video1"
        << LINK
        << "mppvideodec"
        << LINK
        << "rgaconvert output-io-mode=dmabuf-import capture-io-mode=dmabuf"
        << LINK
        << "video/x-raw,format=(string)BGR,width=(int)1920,height=(int)1080,framerate=(fraction)30/1"
        << LINK
        << "appsink name="
        << APPSINK_NAME;

    return pipelineString.str();
}
#endif

static std::string CreateAppSrcPipeline()
{
    std::stringstream pipelineString;

    pipelineString
        << "appsrc caps=video/x-raw,format=(string)BGR,width=(int)1920,height=(int)1080,framerate=(fraction)30/1 "
           "block=true name="
        << APPSRC_NAME
        << LINK
        << "rkximagesink sync=false";

    return pipelineString.str();
}

#endif
