
CXXFLAGS = -Wall -g -I ./include/ 
all: lecturaVideo

lecturaVideo: FocusingMeasures.cpp
	g++ $(CXXFLAGS) `pkg-config --cflags opencv4` -o ejecutable -lfftw3 FocusingMeasures.cpp  `pkg-config --libs opencv4` -lm -lfftw3 

clean:
	rm lecturaVideo



