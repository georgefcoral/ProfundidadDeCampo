
CXXFLAGS =  -Wall -g -I ./include/

all: camera_calibration

camera_calibration: objs/camera_calibration.o
	g++ $(CXXFLAGS) -o camera_calibration objs/camera_calibration.o `pkg-config opencv --libs`

objs/camera_calibration.o: camera_calibration.cpp
	g++ $(CXXFLAGS) -o objs/camera_calibration.o -c camera_calibration.cpp `pkg-config opencv --cflags`

clean: 
	rm objs/*.o camera_calibration 
