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

#include "OpenCVFaceDect.h"

using namespace cv;

OpenCVFaceDect::OpenCVFaceDect()
{
    scale__ = 16;
}

OpenCVFaceDect::~OpenCVFaceDect()
{
}

void OpenCVFaceDect::Initialize(std::string cascade_path)
{
    cascade__.load(cascade_path);
}

void OpenCVFaceDect::Process(void* framebuffer, int width, int height)
{
    std::vector<Rect> faces;
    int i;
    Mat frame(height, width, CV_8UC3, (char*)framebuffer, Mat::AUTO_STEP);
    Mat gray, smallImg(cvRound(frame.rows / scale__), cvRound(frame.cols / scale__), CV_8UC1);

    const static Scalar colors[] = { CV_RGB(0, 0, 255),
        CV_RGB(0, 128, 255),
        CV_RGB(0, 255, 255),
        CV_RGB(0, 255, 0),
        CV_RGB(255, 128, 0),
        CV_RGB(255, 255, 0),
        CV_RGB(255, 0, 0),
        CV_RGB(255, 0, 255) };
    // scale to small picture
    cvtColor(frame, gray, CV_BGR2GRAY);
    resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
    equalizeHist(smallImg, smallImg);

    cascade__.detectMultiScale(smallImg, faces);
    for (std::vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++) {
        std::vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i % 8];
        int radius;

        double aspect_ratio = (double)r->width / r->height;
        if (0.75 < aspect_ratio && aspect_ratio < 1.3) {
            center.x = cvRound((r->x + r->width * 0.5) * scale__);
            center.y = cvRound((r->y + r->height * 0.5) * scale__);
            radius = cvRound((r->width + r->height) * 0.25 * scale__);
            circle(frame, center, radius, color, 3, 8, 0);
        } else
            rectangle(frame, cvPoint(cvRound(r->x * scale__), cvRound(r->y * scale__)),
                cvPoint(cvRound((r->x + r->width - 1) * scale__), cvRound((r->y + r->height - 1) * scale__)),
                color, 3, 8, 0);
    }
}
