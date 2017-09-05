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
#include "PipeLines.h"

#include <sstream>
#include <sys/ioctl.h>
#include <sys/mman.h>

OpenCVStream::OpenCVStream()
{
    is_streaming__ = false;

    OpenCVFaceDect* face_dect = new OpenCVFaceDect();
    face_dect->Initialize("./blobs/haarcascade_frontalface_alt.xml");

    effect_lists.push_back(face_dect);
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
    GstMapInfo map;
    GstStructure* s;
    GstBuffer* buffer;
    GstCaps* caps;
    GstMemory* mem;
    int height, width, size;
    int fd;
    void* map_data;

    while (is_streaming__) {
        if (sink_pipeline__->GetIsNewFrameAvailable()) {
            sink_pipeline__->GetLatestSample(&sample);

            caps = gst_sample_get_caps(sample);
            buffer = gst_sample_get_buffer(sample);
            s = gst_caps_get_structure(caps, 0);
            gst_structure_get_int(s, "height", &height);
            gst_structure_get_int(s, "width", &width);

            size = gst_buffer_get_size(buffer);
            /* Since gstreamer don't support map buffer to write,
             * we have to use mmap directly
             */
            mem = gst_buffer_peek_memory(buffer, 0);
            fd = gst_dmabuf_memory_get_fd(mem);
            map_data = mmap64(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

            std::list<OpenCVEffect*>::iterator itor = effect_lists.begin();
            while (itor != effect_lists.end()) {
                /* assume BGR */
                (*itor)->Process((void*)map_data, width, height);

                itor++;
            }

            munmap(map_data, size);
            /* will auto released */
            gst_buffer_ref(buffer);
            src_pipeline__->SendBUF(buffer);
            sink_pipeline__->ReleaseFrameBuffer();
            /* g_print("%s\n", gst_caps_to_string(caps)); */
        }
    }
}

//gst_sample_ref(&sample);
//gst_sample_unref(&sample);