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

#ifndef _OPENCV_FACEDECT_H
#define _OPENCV_FACEDECT_H

#include "OpenCVEffect.h"

#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

class OpenCVFaceDect : public OpenCVEffect {
public:
    OpenCVFaceDect();
    ~OpenCVFaceDect();

    void Initialize(std::string cascade_path);
    void Process(void* framebuffer, int width, int height);

private:
    cv::CascadeClassifier cascade__;
    double scale__;
};

#endif