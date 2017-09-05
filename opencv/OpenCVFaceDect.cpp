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
    scale__ = 8;

    ocl::setBinaryPath("./blobs/");
}

OpenCVFaceDect::~OpenCVFaceDect()
{
}

void OpenCVFaceDect::Initialize(std::string cascade_path)
{
    if (!cascade__.load(cascade_path)) {
        std::cout << "ERROR: Load cascade failed" << std::endl;
    }
}

void OpenCVFaceDect::Update(std::vector<Rect> &faces)
{
    boost::lock_guard<boost::mutex> guard(face_mutex__);
    last_face__ = faces;
}

void OpenCVFaceDect::Run()
{
    boost::lock_guard<boost::mutex> guard(thread_mutex__);
    Mat smallImg(cvRound(gray__.rows / scale__), cvRound(gray__.cols / scale__), CV_8UC1);
    std::vector<Rect> faces;
    double t = (double)cvGetTickCount();

    // scale to small picture
    resize(gray__, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
    equalizeHist(smallImg, smallImg);

    cascade__.detectMultiScale(smallImg, faces, 1.1,
        1, 0 | CV_HAAR_SCALE_IMAGE, Size(10, 10), Size(0, 0));

    t = (double)cvGetTickCount() - t;
    printf("detection time = %g ms\n", t / ((double)cvGetTickFrequency() * 1000.));

    Update(faces);
}

void OpenCVFaceDect::Process(void* framebuffer, int width, int height)
{
    Mat frame(height, width, CV_8UC3, (char*)framebuffer, Mat::AUTO_STEP);

    if(thread_mutex__.try_lock()) {
        cvtColor(frame, gray__, CV_BGR2GRAY);
        process_thread__ = boost::thread(&OpenCVFaceDect::Run, this);
        thread_mutex__.unlock();
    }

    Draw(framebuffer, width, height);
}

void OpenCVFaceDect::Draw(void *framebuffer, int width, int height)
{
    boost::lock_guard<boost::mutex> guard(face_mutex__);
    Mat frame(height, width, CV_8UC3, (char*)framebuffer, Mat::AUTO_STEP);
    int i;

    const static Scalar colors[] = { CV_RGB(0, 0, 255),
        CV_RGB(0, 128, 255),
        CV_RGB(0, 255, 255),
        CV_RGB(0, 255, 0),
        CV_RGB(255, 128, 0),
        CV_RGB(255, 255, 0),
        CV_RGB(255, 0, 0),
        CV_RGB(255, 0, 255) };

    for (std::vector<Rect>::const_iterator r = last_face__.begin(); r != last_face__.end(); r++, i++) {
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
