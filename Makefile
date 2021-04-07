
CXXFLAGS = -Wall -g -I ./include/ 
all: FourierDescriptorsFocusing FocusingMeasures_MOLIU FocusingMeasures_SUBB1

FocusingMeasures_MOLIU: FocusingMeasures.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures -lfftw3 FocusingMeasures.cpp  `pkg-config --libs opencv4` -lm -lfftw3 
FocusingMeasures_SUBB1: FocusingMeasures2.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutableFocusingMeasures2 -lfftw3 FocusingMeasures2.cpp  `pkg-config --libs opencv4` -lm -lfftw3 
FourierDescriptorsFocusing: objs/Descriptors.o objs/FourierDescriptor.o
	g++ $(CXXFLAGS) -o ejecutableDescriptors objs/Descriptors.o objs/FourierDescriptor.o `pkg-config opencv4 --libs` -lfftw3
objs/Descriptors.o: Descriptors.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -o objs/Descriptors.o -c Descriptors.cpp `pkg-config opencv4 --cflags`
objs/FourierDescriptor.o: FourierDescriptor.cpp include/FourierDescriptor.h
	g++ $(CXXFLAGS) -c -o objs/FourierDescriptor.o FourierDescriptor.cpp `pkg-config opencv4 --cflags`
clean:
	rm FocusingMeasures_MOLIU
	rm FocusingMeasures_SUBB1
	rm FourierDescriptorsFocusing
	rm objs/*.o Descriptors FourierDescriptor


