
CXXFLAGS =  -Wall -g -I ./include -I ../Camera1394/include  -L../Camera1394/lib 

all: Example1394-OpenCV main PuntoDeFuga2 objs/deepFunctions2.o objs/PuntoDeFuga2.o
 
Example1394-OpenCV: objs/Example1394-OpenCV.o
	g++ $(CXXFLAGS) -o Example1394-OpenCV objs/Example1394-OpenCV.o `pkg-config opencv --libs` -lpthread -lCamera1394 -ldc1394 -lm -lz

objs/Example1394-OpenCV.o: Example1394-OpenCV.cpp
	g++ $(CXXFLAGS) -o objs/Example1394-OpenCV.o -c Example1394-OpenCV.cpp `pkg-config opencv --cflags`

main: main.cpp serialPOSIX.o
	g++ $(CXXFLAGS) -o main main.cpp serialPOSIX.o `pkg-config opencv --libs` -lpthread -lCamera1394 -ldc1394 -lm -lz

serialPOSIX.o:serialPOSIX.cpp
	g++ $(CXXFLAGS) -c serialPOSIX.cpp
	
PuntoDeFuga2: objs/PuntoDeFuga2.o objs/deepFunctions2.o objs/sortContours.o
	g++ $(CXXFLAGS) -o PuntoDeFuga2 objs/PuntoDeFuga2.o objs/deepFunctions2.o objs/sortContours.o `pkg-config opencv --libs`
	
objs/deepFunctions2.o: deepFunctions2.cpp 
	g++ $(CXXFLAGS) -o objs/deepFunctions2.o -c deepFunctions2.cpp  `pkg-config opencv --cflags`

objs/PuntoDeFuga2.o: PuntoDeFuga2.cpp 
	g++ $(CXXFLAGS) -o objs/PuntoDeFuga2.o -c PuntoDeFuga2.cpp  `pkg-config opencv --cflags`

objs/sortContours.o: sortContours.cpp include/sortContours.h
	g++ $(CXXFLAGS) -c -o objs/sortContours.o sortContours.cpp `pkg-config opencv --cflags`

clean: 
	rm objs/*.o Example1394-OpenCV main serialPOSIX.o
	rm PuntoDeFuga2
