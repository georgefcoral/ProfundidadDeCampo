
CXXFLAGS = -Wall -g -I ./include/ 
all: lecturaVideo

lecturaVideo: FocusingMeasuresFourier.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutable -lfftw3 FocusingMeasuresFourier.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

clean:
	rm lecturaVideo



