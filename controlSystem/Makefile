all: main

main: main.cpp serialPOSIX.o
	g++ -Wall -I ./ -o main main.cpp serialPOSIX.o

serialPOSIX.o:serialPOSIX.cpp
	g++ -Wall -I ./ -c serialPOSIX.cpp

clean:
	rm main serialPOSIX.o
