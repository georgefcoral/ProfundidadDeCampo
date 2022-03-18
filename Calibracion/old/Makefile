
OPENCV_VERSION=`pkg-config --modversion opencv | sed 's/^\(.\).*/\1/'`
CXXFLAGS =  -D"__OCV_VER__=$(OPENCV_VERSION)" -Wall -g -I ../include/ 

all: camera_calibration CaptureFrames correctImages

camera_calibration: objs/camera_calibration.o
	g++ $(CXXFLAGS) -o camera_calibration objs/camera_calibration.o `pkg-config opencv --libs`

objs/camera_calibration.o: camera_calibration.cpp
	g++ $(CXXFLAGS) -o objs/camera_calibration.o -c camera_calibration.cpp `pkg-config opencv --cflags`

CaptureFrames: objs/CaptureFrames.o
	g++ $(CXXFLAGS) -o CaptureFrames objs/CaptureFrames.o `pkg-config opencv --libs`

objs/CaptureFrames.o: CaptureFrames.cpp
	g++ $(CXXFLAGS) -o objs/CaptureFrames.o -c CaptureFrames.cpp `pkg-config opencv --cflags`

correctImages: objs/correctImages.o
	g++ $(CXXFLAGS) -o correctImages objs/correctImages.o `pkg-config opencv --libs`

objs/correctImages.o: correctImages.cpp
	g++ $(CXXFLAGS) -o objs/correctImages.o -c correctImages.cpp `pkg-config opencv --cflags`

clean: 
	rm objs/*.o camera_calibration camera_calibration correctImages
