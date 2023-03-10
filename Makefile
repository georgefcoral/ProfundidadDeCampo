
#CXXFLAGS = -Wall -g -D__VERBOSE__ -I ./include/ 
CXXFLAGS = -Wall -g -I ./include/ 

all: ejecutableFocusingMeasures ejecutableFocusingMeasures2 PuntoDeFuga2 PuntoDeFuga3 testSortContours testMomentsMetricA testMomentsMetricB

ejecutableFocusingMeasures: FocusingMeasures.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures -lfftw3 FocusingMeasures.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

ejecutableFocusingMeasures2: FocusingMeasures2.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures2 -lfftw3 FocusingMeasures2.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

PuntoDeFuga2: objs/PuntoDeFuga2.o objs/deepFunctions2.o objs/sortContours.o 
	g++ $(CXXFLAGS) -o PuntoDeFuga2 objs/PuntoDeFuga2.o objs/deepFunctions2.o objs/sortContours.o `pkg-config opencv4 --libs`

PuntoDeFuga3: objs/PuntoDeFuga3.o objs/deepFunctions2.o objs/sortContours.o 
	g++ $(CXXFLAGS) -o PuntoDeFuga3 objs/PuntoDeFuga3.o objs/deepFunctions2.o objs/sortContours.o `pkg-config opencv4 --libs`

testSortContours: objs/testSortContours.o objs/sortContours.o
	g++ $(CXXFLAGS) -o testSortContours objs/testSortContours.o objs/sortContours.o `pkg-config opencv4 --libs`

testMomentsMetricA: objs/testMomentsMetricA.o objs/sortContours.o
	g++ $(CXXFLAGS) -o testMomentsMetricA objs/testMomentsMetricA.o objs/sortContours.o `pkg-config opencv4 --libs`

testMomentsMetricB: objs/testMomentsMetricB.o objs/sortContours.o
	g++ $(CXXFLAGS) -o testMomentsMetricB objs/testMomentsMetricB.o objs/sortContours.o `pkg-config opencv4 --libs`

objs/PuntoDeFuga2.o: PuntoDeFuga2.cpp include/FeatureTracker.h include/Mosaic.h
	g++ $(CXXFLAGS) -o objs/PuntoDeFuga2.o -c PuntoDeFuga2.cpp  `pkg-config opencv4 --cflags`

objs/PuntoDeFuga3.o: PuntoDeFuga3.cpp include/FeatureTracker.h include/Mosaic.h
	g++ $(CXXFLAGS) -o objs/PuntoDeFuga3.o -c PuntoDeFuga3.cpp  `pkg-config opencv4 --cflags`

objs/testSortContours.o: testSortContours.cpp 
	g++ $(CXXFLAGS) -o objs/testSortContours.o -c testSortContours.cpp `pkg-config opencv4 --cflags`

objs/testMomentsMetricA.o: testMomentsMetricA.cpp 
	g++ $(CXXFLAGS) -o objs/testMomentsMetricA.o -c testMomentsMetricA.cpp `pkg-config opencv4 --cflags`

objs/testMomentsMetricB.o: testMomentsMetricB.cpp 
	g++ $(CXXFLAGS) -o objs/testMomentsMetricB.o -c testMomentsMetricB.cpp `pkg-config opencv4 --cflags`

objs/deepFunctions2.o: deepFunctions2.cpp 
	g++ $(CXXFLAGS) -o objs/deepFunctions2.o -c deepFunctions2.cpp  `pkg-config opencv4 --cflags`

objs/FourierDescriptor.o: FourierDescriptor.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -c -o objs/FourierDescriptor.o FourierDescriptor.cpp `pkg-config opencv4 --cflags`

objs/sortContours.o: sortContours.cpp include/sortContours.h
	g++ $(CXXFLAGS) -c -o objs/sortContours.o sortContours.cpp `pkg-config opencv4 --cflags`

clean:
	rm ejecutableFocusingMeasures
	rm ejecutableFocusingMeasures2
	rm PuntoDeFuga2
	rm PuntoDeFuga3
	rm testSortContours 
	rm testMomentsMetricA
	rm testMomentsMetricB
	rm objs/*.o


