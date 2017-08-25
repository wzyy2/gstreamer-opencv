#CXX=g++
CXX=arm-linux-gnueabihf-g++
#export PKG_CONFIG_PATH=/usr/lib/arm-linux-gnueabihf/pkgconfig:$PKG_CONFIG_PATH

GST_FLAGS=$(shell pkg-config --cflags --libs gstreamer-1.0)
OPENCV_FLAGS=$(shell pkg-config --cflags --libs opencv)

CFLAGS = $(GST_FLAGS) $(OPENCV_FLAGS)

LIBS := -L/usr/lib/arm-linux-gnueabihf -lglib-2.0 -lgobject-2.0 -lopencv_imgproc -lopencv_highgui -lopencv_core -lopencv_calib3d 
LIBS += -lgstapp-1.0 -lgstreamer-1.0 -lgstbase-1.0 -lgobject-2.0 -lglib-2.0 -lgstvideo-1.0 -lgstallocators-1.0
LIBS += -lboost_thread  -lboost_system
 
define all-cpp-files-under
$(shell find $(1) -name "*."$(2) -and -not -name ".*" )
endef

define all-subdir-cpp-files
$(call all-cpp-files-under,.,"cpp")
endef

CPPSRCS	 = $(call all-subdir-cpp-files)

CPPOBJS	:= $(CPPSRCS:.cpp=.o)

all: gst_opencv

gst_opencv: $(CPPOBJS)
	$(CXX) $(CFLAGS) $(CPPOBJS) -o gst_opencv $(LIBS)

$(CPPOBJS) : %.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f opencv/*.o gst/*.o *.o gst_opencv
