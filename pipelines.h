#ifndef _PIPELINE_H
#define _PIPELINE_H

#define TEST_VIDEO
// #define TEST_RTSP
// #define TEST_CAMERA

#ifndef  TEST_CAMERA
static std::string CreateAppSinkPipeline()
{
    std::stringstream pipelineString;

    pipelineString
#ifdef TEST_RTSP
        << "rtsp://admin:M60090904@172.16.1.82:554/"
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
        << "v4l2video0convert output-io-mode=dmabuf capture-io-mode=dmabuf"
        << LINK
        << "video/x-raw,format=RGB,width=1920,height=1080"
        << LINK
        << "queue"
        << LINK
        << "appsink name=" << APPSINK_NAME;

    return pipelineString.str();
}
#else
static std::string CreateAppSinkPipeline()
{
    std::stringstream pipelineString;

    pipelineString
        << "v4l2src device=/dev/video1 io-mode=4"
        << LINK
        << "mppvideodec"
        << LINK
        << "v4l2video0convert output-io-mode=dmabuf capture-io-mode=dmabuf"
        << LINK
        << "video/x-raw,format=RGB,width=1920,height=1080"
        << LINK
        << "appsink name=" << APPSINK_NAME;

    return pipelineString.str();
}
#endif

static std::string CreateAppSrcPipeline()
{
    std::stringstream pipelineString;

    pipelineString
	<< "appsrc name=" << APPSRC_NAME << " block=true" 
        << LINK
        << "video/x-raw,format=(string)RGB,width=(int)1920,height=(int)1080,framerate=(fraction)30/1"
        << LINK
        << "rkximagesink";

    return pipelineString.str();
}

#endif