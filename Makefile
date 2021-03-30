

all: lecturaVideo

lecturaVideo: FocusingMeasures.cpp
	g++ -Wall `pkg-config --cflags opencv4` -o ejecutable FocusingMeasures.cpp `pkg-config --libs opencv4`

clean:
	rm lecturaVideo



