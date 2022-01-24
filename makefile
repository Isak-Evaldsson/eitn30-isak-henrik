.PHONY: all clean

all:
	g++ src/getting_started.cpp -lrf24 -lpthread -g
	sudo ./a.out

clean:
	rm a.out
