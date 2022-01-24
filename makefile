.PHONY: all clean

all:
	g++ src/transmitter.cpp -lrf24 -lpthread -g
	sudo ./a.out

r: 
	g++ src/reciver.cpp -lrf24 -lpthread -g
	sudo ./a.out

t:
	g++ src/transmitter.cpp -lrf24 -lpthread -g
	sudo ./a.out

clean:
	rm a.out
