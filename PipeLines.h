#ifndef _PIPELINE_H
#define _PIPELINE_H

#define TEST_VIDEO
//#define TEST_RTSP
//#define TEST_CAMERA

/*
Kernel Patch for wrong color(mppvideodec):

driver/media/platform/rockchip-rga/rga-hw.c
@@ -73,10 +73,16 @@ rga_get_addr_offset(struct rga_frame *frm, unsigned int x, unsigned int y,
    lt->y_off = y * frm->stride + x * pixel_width;
    lt->u_off =
        frm->width * frm->height + (y / y_div) * uv_stride + x / x_div;
    lt->v_off = lt->u_off + frm->width * frm->height / uv_factor;

+	if(frm->fmt->fourcc == V4L2_PIX_FMT_NV12);
+		lt->u_off = DIV_ROUND_UP(frm->width, 32) *
+			DIV_ROUND_UP(frm->height, 32) +
+			(y / y_div) * uv_stride + x / x_div;
+
    lb->y_off = lt->y_off + (h - 1) * frm->stride;
    lb->u_off = lt->u_off + (h / y_div - 1) * uv_stride;
    lb->v_off = lt->v_off + (h / y_div - 1) * uv_stride;
*/

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
        << "rgaconvert output-io-mode=dmabuf-import capture-io-mode=dmabuf"
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
