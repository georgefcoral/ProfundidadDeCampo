
CXXFLAGS = -Wall -g -I ./include/ 
all: ejecutableFocusingMeasures ejecutableFocusingMeasures2 PuntoDeFuga2

ejecutableFocusingMeasures: FocusingMeasures.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures -lfftw3 FocusingMeasures.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

ejecutableFocusingMeasures2: FocusingMeasures2.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures2 -lfftw3 FocusingMeasures2.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

PuntoDeFuga2: objs/PuntoDeFuga2.o objs/deepFunctions2.o
	g++ $(CXXFLAGS) -o PuntoDeFuga2 objs/PuntoDeFuga2.o objs/deepFunctions2.o `pkg-config opencv4 --libs` -lfftw3

objs/PuntoDeFuga2.o: PuntoDeFuga2.cpp 
	g++ $(CXXFLAGS) -o objs/PuntoDeFuga2.o -c PuntoDeFuga2.cpp  `pkg-config opencv4 --cflags`

objs/deepFunctions2.o: deepFunctions2.cpp 
	g++ $(CXXFLAGS) -o objs/deepFunctions2.o -c deepFunctions2.cpp  `pkg-config opencv4 --cflags`

objs/FourierDescriptor.o: FourierDescriptor.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -c -o objs/FourierDescriptor.o FourierDescriptor.cpp `pkg-config opencv4 --cflags`

clean:
	rm ejecutableFocusingMeasures
	rm ejecutableFocusingMeasures2
	rm PuntoDeFuga2
	rm objs/*.o


