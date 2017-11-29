## Introduction

Gstreamer + OpenCV Demo application.  
ZERO-COPY Optimization.  
Suitable for RockChip Platform.

You could change pipeline by editing `pipelines.h`.

pipeline:  
rtsp/video/camera ->(h264/mjpeg) -> mppvideodec -> (nv12) -> rga videoconvert -> (rgb) -> appsink  
appsrc ->(opencv process rgb) -> rkximagesink  

Assume:  
* rga = /dev/video0
* camera = /dev/video1
* videofile = /usr/local/test.mp4

#### More in Blog

Welcome Comments :-p

http://blog.iotwrt.com/media/2017/08/23/opencv-gstreamer/
