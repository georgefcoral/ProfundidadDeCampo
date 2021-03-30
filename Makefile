

all: lecturaVideo

lecturaVideo: main.cpp
	g++ -Wall `pkg-config --cflags opencv4` -o test main.cpp `pkg-config --libs opencv4`

clean:
	rm lecturaVideo



