.PHONY: all clean

all:
	g++ -I./rf24libs/RF24 src/test.cpp
	./a.out

clean:
	rm a.out
