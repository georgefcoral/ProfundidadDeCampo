
CXXFLAGS = -Wall -g -I ./include/ 
all: ejecutableFocusingMeasures ejecutableFocusingMeasures2 ejecutableDescriptors

ejecutableFocusingMeasures: FocusingMeasures.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures -lfftw3 FocusingMeasures.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

ejecutableFocusingMeasures2: FocusingMeasures2.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures2 -lfftw3 FocusingMeasures2.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

ejecutableDescriptors: objs/Descriptors.o objs/FourierDescriptor.o objs/deepFunctions.o
	g++ $(CXXFLAGS) -o ejecutableDescriptors objs/Descriptors.o objs/FourierDescriptor.o `pkg-config opencv4 --libs` -lfftw3

objs/Descriptors.o: PuntoDeFuga.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -o objs/Descriptors.o -c PuntoDeFuga.cpp  `pkg-config opencv4 --cflags`

objs/deepFunctions.o: deepFunctions.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -o objs/deepFunctions.o -c deepFunctions.cpp  `pkg-config opencv4 --cflags`

objs/FourierDescriptor.o: FourierDescriptor.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -c -o objs/FourierDescriptor.o FourierDescriptor.cpp `pkg-config opencv4 --cflags`

clean:
	rm ejecutableFocusingMeasures
	rm ejecutableFocusingMeasures2
	rm ejecutableDescriptors
	rm objs/*.o Descriptors FourierDescriptor


