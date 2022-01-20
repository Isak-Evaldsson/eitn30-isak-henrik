.PHONY: all clean

all:
	g++ src/test.cpp -lrf24 -lpthread -g
	sudo ./a.out

clean:
	rm a.out
