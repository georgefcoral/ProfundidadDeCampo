
CXXFLAGS =  -Wall -g -I ../Camera1394/include -L../Camera1394/lib

all: Example1394-OpenCV

Example1394-OpenCV: objs/Example1394-OpenCV.o
	g++ $(CXXFLAGS) -o Example1394-OpenCV objs/Example1394-OpenCV.o `pkg-config opencv --libs` -lpthread -lCamera1394 -ldc1394 -lm -lz

objs/Example1394-OpenCV.o: Example1394-OpenCV.cpp
	g++ $(CXXFLAGS) -o objs/Example1394-OpenCV.o -c Example1394-OpenCV.cpp `pkg-config opencv --cflags`

clean: 
	rm objs/*.o Example1394-OpenCV 
