/**
 * Copyright (C) 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Author: Jacob Chen <jacob-chen@iotwrt.com>
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "OpenCVStream.h"
#include "pipelines.h"

#include <sstream>

OpenCVStream::OpenCVStream()
{
    is_streaming__ = false;
}

OpenCVStream::~OpenCVStream()
{
    if (is_streaming__) {
        StreamOFF();
    }
}

void OpenCVStream::StreamON()
{
    boost::lock_guard<boost::mutex> guard(state_mutex__);

    if (is_streaming__) {
        return;
    }

    sink_pipeline__ = new GstAppSinkPipeline();
    sink_pipeline__->Initialize(CreateAppSinkPipeline());
    sink_pipeline__->set_is_streaming(true);
    sink_pipeline__->SetPipelineState(GST_STATE_PLAYING);

    src_pipeline__ = new GstAppSrcPipeline();
    src_pipeline__->Initialize(CreateAppSrcPipeline());
    src_pipeline__->SetPipelineState(GST_STATE_PLAYING);

    process_thread__ = boost::thread(&OpenCVStream::Process, this);
    is_streaming__ = true;
}

void OpenCVStream::StreamOFF()
{
    int i = 0;
    boost::lock_guard<boost::mutex> guard(state_mutex__);

    if (!is_streaming__) {
        return;
    }

    is_streaming__ = false;
    process_thread__.join();
    src_pipeline__->SetPipelineState(GST_STATE_PAUSED);
    sink_pipeline__->SetPipelineState(GST_STATE_PAUSED);
    sink_pipeline__->set_is_streaming(false);
    delete src_pipeline__;
    delete sink_pipeline__;
}

void OpenCVStream::Process()
{
    GstSample* sample;
    int height, width;

    while (is_streaming__) {
        if (sink_pipeline__->GetIsNewFrameAvailable()) {
            sink_pipeline__->GetLatestSample(&sample);
            GstCaps* caps = gst_sample_get_caps(sample);
            GstBuffer* buffer = gst_sample_get_buffer(sample);

            // GstMemory *mem = gst_buffer_peek_memory (buffer, 0);
            // if (!gst_is_dmabuf_memory(mem)) {
            //     printf("dsadsasdadadsasdas\n");
            // } else {
            //     printf("gst_is_dmabuf_memory\n");
            // }
            // while(1);
            // will auto released
            gst_buffer_ref(buffer);
            src_pipeline__->SendBUF(buffer);

        } else {
            sink_pipeline__->ReleaseFrameBuffer();
        }
    }
}
