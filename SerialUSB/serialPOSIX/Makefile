all: test1 mirror

test1: test1.cpp serialPOSIX.o
	g++ -Wall -I ./ -o test1 test1.cpp serialPOSIX.o

mirror: mirror.cpp serialPOSIX.o
	g++ -Wall -I ./ -o mirror mirror.cpp serialPOSIX.o

serialPOSIX.o:serialPOSIX.cpp
	g++ -Wall -I ./ -c serialPOSIX.cpp

clean:
	rm test1 mirror serialPOSIX.o
