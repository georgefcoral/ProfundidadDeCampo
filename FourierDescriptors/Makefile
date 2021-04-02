
CXXFLAGS = -Wall -g -I ./include/ 

all: exampleFourierDescriptors exampleHuMomentsFourierDescriptors


exampleFourierDescriptors: objs/exampleFourierDescriptors.o objs/FourierDescriptor.o
	g++ $(CXXFLAGS) -o exampleFourierDescriptors objs/exampleFourierDescriptors.o objs/FourierDescriptor.o `pkg-config opencv4 --libs` -lfftw3

exampleHuMomentsFourierDescriptors: objs/exampleHuMomentsFourierDescriptors.o objs/FourierDescriptor.o
	g++ $(CXXFLAGS) -o exampleHuMomentsFourierDescriptors objs/exampleHuMomentsFourierDescriptors.o objs/FourierDescriptor.o `pkg-config opencv4 --libs` -lfftw3

objs/exampleFourierDescriptors.o: exampleFourierDescriptors.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -o objs/exampleFourierDescriptors.o -c exampleFourierDescriptors.cpp `pkg-config opencv4 --cflags`

objs/exampleHuMomentsFourierDescriptors.o: exampleHuMomentsFourierDescriptors.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -o objs/exampleHuMomentsFourierDescriptors.o -c exampleHuMomentsFourierDescriptors.cpp `pkg-config opencv4 --cflags`

objs/FourierDescriptor.o: FourierDescriptor.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -c -o objs/FourierDescriptor.o FourierDescriptor.cpp `pkg-config opencv4 --cflags`

clean: 
	rm objs/*.o exampleHuMomentsFourierDescriptors exampleFourierDescriptors
