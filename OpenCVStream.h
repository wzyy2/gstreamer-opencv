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

#ifndef _OPENCV_APPSINK_H
#define _OPENCV_APPSINK_H

#include "gst_opencv.h"

class OpenCVStream{
public:
    OpenCVStream();
    ~OpenCVStream();

    void StreamON();
    void StreamOFF();

    void Process();
private:
    bool is_streaming__;
    GstAppSinkPipeline *sink_pipeline__;
    GstAppSrcPipeline *src_pipeline__;
    boost::thread process_thread__;
    boost::mutex state_mutex__;
};

#endif